#pragma once
#include <Windows.h>
#include <cstdint>
#include <TlHelp32.h>
#include <vector>
#include <cstring>
#include <iostream>
#include "../protections/lazy.h"
#include "../protections/sk.h"

#define attach_code             CTL_CODE(FILE_DEVICE_UNKNOWN, 0x850, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define read_code               CTL_CODE(FILE_DEVICE_UNKNOWN, 0x851, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define get_module_by_peb_code  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x853, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define get_image_base_code     CTL_CODE(FILE_DEVICE_UNKNOWN, 0x854, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define get_peb_code            CTL_CODE(FILE_DEVICE_UNKNOWN, 0x855, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define batch_read_code         CTL_CODE(FILE_DEVICE_UNKNOWN, 0x860, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define page_walk_read_code     CTL_CODE(FILE_DEVICE_UNKNOWN, 0x981, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

typedef struct _Request {
    HANDLE process_id;
    HANDLE WriteRequest;
    PVOID target;
    PVOID buffer;
    SIZE_T size;
} Request, * PRequest;

typedef struct _MODULE_PACK_KM {
    UINT32 pid;
    UINT64 baseAddress;
    WCHAR moduleName[1024];
} MODULE_PACK_KM, * PMODULE_PACK_KM;

typedef struct _PEB_PACK_KM {
    UINT32 pid;
    UINT64 peb;
} PEB_PACK_KM, * PPEB_PACK_KM;

typedef struct _BatchReadHeader {
    HANDLE process_id;
    UINT32 num_requests;
    SIZE_T total_buffer_size;
} BatchReadHeader, * PBatchReadHeader;

typedef struct _BatchReadRequest {
    DWORD64 address;
    SIZE_T size;
    SIZE_T offset_in_buffer;
} BatchReadRequest, * PBatchReadRequest;

namespace slime {

    inline HANDLE h_driver = INVALID_HANDLE_VALUE;
    inline HANDLE h_process = nullptr;
    inline uintptr_t base = 0;
    inline uintptr_t peb = 0;

    constexpr uint64_t min_user_addr = 0x0000000000010000;
    constexpr uint64_t max_user_addr = 0x00007FFFFFFFFFFF;

    inline bool valid_address(uint64_t address) {
        return address >= min_user_addr && address <= max_user_addr;
    }

    bool initialize_driver(const char* device_path, UINT32 target_pid) {
        h_driver = LI_FN(CreateFileA)(
            device_path,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            nullptr,
            OPEN_EXISTING,
            0,
            nullptr
        );

        if (h_driver == INVALID_HANDLE_VALUE)
            return false;

        h_process = (HANDLE)(ULONG_PTR)target_pid;

        Request attach_req = { 0 };
        attach_req.process_id = h_process;

        DWORD bytes_returned = 0;
        LI_FN(DeviceIoControl)(
            h_driver,
            attach_code,
            &attach_req,
            sizeof(attach_req),
            &attach_req,
            sizeof(attach_req),
            &bytes_returned,
            nullptr
        );

        return true;
    }

    uint64_t find_module(const wchar_t* module_name) {
        if (h_driver == INVALID_HANDLE_VALUE)
            return 0;

        MODULE_PACK_KM pack = { 0 };
        pack.pid = (UINT32)(ULONG_PTR)h_process;
        wcsncpy_s(pack.moduleName, module_name, _TRUNCATE);

        DWORD bytes_returned = 0;
        if (!DeviceIoControl(
            h_driver,
            get_module_by_peb_code,
            &pack,
            sizeof(pack),
            &pack,
            sizeof(pack),
            &bytes_returned,
            nullptr
        )) {
            return 0;
        }

        return pack.baseAddress;
    }

    uint64_t get_process_base() {
        if (h_driver == INVALID_HANDLE_VALUE)
            return 0;

        MODULE_PACK_KM pack = { 0 };
        pack.pid = (UINT32)(ULONG_PTR)h_process;

        DWORD bytes_returned = 0;
        if (!DeviceIoControl(
            h_driver,
            get_image_base_code,
            &pack,
            sizeof(pack),
            &pack,
            sizeof(pack),
            &bytes_returned,
            nullptr
        )) {
            return 0;
        }

        return pack.baseAddress;
    }

    uint64_t get_process_peb() {
        if (h_driver == INVALID_HANDLE_VALUE)
            return 0;

        PEB_PACK_KM pack = { 0 };
        pack.pid = (UINT32)(ULONG_PTR)h_process;

        DWORD bytes_returned = 0;
        if (!DeviceIoControl(
            h_driver,
            get_peb_code,
            &pack,
            sizeof(pack),
            &pack,
            sizeof(pack),
            &bytes_returned,
            nullptr
        )) {
            return 0;
        }

        return pack.peb;
    }

    bool rpm(uint64_t address, void* buffer, SIZE_T size) {
        if (h_driver == INVALID_HANDLE_VALUE)
            return false;

        if (!valid_address(address))
            return false;

        Request req = { 0 };
        req.process_id = h_process;
        req.target = (PVOID)address;
        req.buffer = nullptr;
        req.size = size;

        DWORD bytes_returned = 0;
        BOOL result = DeviceIoControl(
            h_driver,
            read_code,
            &req,
            sizeof(req),
            buffer,
            (DWORD)size,
            &bytes_returned,
            nullptr
        );

        return result && (bytes_returned == size);
    }

    bool rpm_physical(uint64_t address, void* buffer, SIZE_T size) {
        if (h_driver == INVALID_HANDLE_VALUE)
            return false;

        if (!valid_address(address))
            return false;

        Request req = { 0 };
        req.process_id = h_process;
        req.target = (PVOID)address;
        req.buffer = nullptr;
        req.size = size;

        DWORD bytes_returned = 0;
        BOOL result = DeviceIoControl(
            h_driver,
            page_walk_read_code,
            &req,
            sizeof(req),
            buffer,
            (DWORD)size,
            &bytes_returned,
            nullptr
        );

        return result && (bytes_returned > 0);
    }

    bool rpm_batch(const std::vector<std::pair<uint64_t, SIZE_T>>& addresses, void* output, SIZE_T output_size) {
        if (h_driver == INVALID_HANDLE_VALUE || addresses.empty())
            return false;

        SIZE_T header_size = sizeof(BatchReadHeader);
        SIZE_T requests_size = addresses.size() * sizeof(BatchReadRequest);
        SIZE_T total_input = header_size + requests_size;

        std::vector<BYTE> input_data(total_input);

        PBatchReadHeader header = (PBatchReadHeader)input_data.data();
        header->process_id = h_process;
        header->num_requests = (UINT32)addresses.size();
        header->total_buffer_size = output_size;

        PBatchReadRequest requests = (PBatchReadRequest)(input_data.data() + sizeof(BatchReadHeader));

        SIZE_T offset = 0;
        for (size_t i = 0; i < addresses.size(); i++) {
            requests[i].address = addresses[i].first;
            requests[i].size = addresses[i].second;
            requests[i].offset_in_buffer = offset;
            offset += addresses[i].second;
        }

        if (offset > output_size)
            return false;

        DWORD bytes_returned = 0;
        BOOL result = DeviceIoControl(
            h_driver,
            batch_read_code,
            input_data.data(),
            (DWORD)total_input,
            output,
            (DWORD)output_size,
            &bytes_returned,
            nullptr
        );

        return result && (bytes_returned > 0);
    }

    void cleanup() {
        if (h_driver != INVALID_HANDLE_VALUE) {
            LI_FN(CloseHandle)(h_driver);
            h_driver = INVALID_HANDLE_VALUE;
        }
    }
}

namespace mem {

    INT32 get_process_id(const char* process_name) {
        PROCESSENTRY32 entry = { 0 };
        entry.dwSize = sizeof(entry);

        HANDLE snapshot = LI_FN(CreateToolhelp32Snapshot)(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE)
            return 0;

        if (LI_FN(Process32First)(snapshot, &entry)) {
            do {
                if (_stricmp(entry.szExeFile, process_name) == 0) {
                    LI_FN(CloseHandle)(snapshot);
                    return entry.th32ProcessID;
                }
            } while (LI_FN(Process32Next)(snapshot, &entry));
        }

        LI_FN(CloseHandle)(snapshot);
        return 0;
    }

    template<typename T>
    T read(uint64_t address) {
        T value = {};

        if (!slime::rpm(address, &value, sizeof(T)))
            return T{};

        return value;
    }

    template<typename T>
    T read_physical(uint64_t address) {
        T value = {};
        if (!slime::rpm_physical(address, &value, sizeof(T)))
            return T{};
        return value;
    }

    template<typename T>
    T read_chain(uint64_t base, const std::vector<uint64_t>& offsets) {
        uint64_t current = base;
        for (size_t i = 0; i + 1 < offsets.size(); ++i) {
            current = read<uint64_t>(current + offsets[i]);
            if (!current || !slime::valid_address(current))
                return T{};

        }
        return read<T>(current + offsets.back());
    }

    bool attach(const char* process_name) {
        UINT32 pid = get_process_id(process_name);
        if (!pid) {
            std::cout << skCrypt("Process not found: ") << process_name << std::endl;
            return false;
        }

        /*std::cout << skCrypt("PID: ") << pid << std::endl;

        if (!slime::initialize_driver(skCrypt("\\\\.\\slimegetem"), pid)) {
            std::cout << skCrypt("Failed to open driver handle") << std::endl;
            return false;
        }

        std::cout << skCrypt("Driver initialized successfully") << std::endl;

        slime::base = slime::get_process_base();
        if (!slime::base) {
            std::cout << skCrypt("Failed to get base address") << std::endl;
            return false;
        }

        slime::peb = slime::get_process_peb();*/

        return true;
    }
}