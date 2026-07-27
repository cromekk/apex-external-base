#pragma once
#include <windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <string>

class Kmem
{
private:
    HANDLE hProcess = NULL;
    DWORD pid = 0;
    uintptr_t base_address = 0;
    uintptr_t cr3 = 0;

public:
    auto setup(const wchar_t* process_name) -> bool
    {
        pid = GetProcessPid(process_name);
        if (!pid) return false;
        hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
        if (!hProcess) return false;
        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
        if (hSnap != INVALID_HANDLE_VALUE) {
            MODULEENTRY32W modEntry{};
            modEntry.dwSize = sizeof(modEntry);
            if (Module32FirstW(hSnap, &modEntry)) {
                do {
                    if (_wcsicmp(modEntry.szModule, process_name) == 0) {
                        base_address = (uintptr_t)modEntry.modBaseAddr;
                        break;
                    }
                } while (Module32NextW(hSnap, &modEntry));
            }
            CloseHandle(hSnap);
        }
        return (base_address != 0);
    }

    auto GetProcessPid(const wchar_t* process_name) -> DWORD
    {
        DWORD proc_id = 0;
        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnap != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32W procEntry{};
            procEntry.dwSize = sizeof(procEntry);
            if (Process32FirstW(hSnap, &procEntry)) {
                do {
                    if (_wcsicmp(procEntry.szExeFile, process_name) == 0) {
                        proc_id = procEntry.th32ProcessID;
                        break;
                    }
                } while (Process32NextW(hSnap, &procEntry));
            }
            CloseHandle(hSnap);
        }
        pid = proc_id;
        return proc_id;
    }

    auto Pid() const -> DWORD { return pid; }
    auto GetBase() const -> uintptr_t { return base_address; }
    auto GetCR3() const -> uintptr_t { return cr3; }

    template <typename T>
    auto read(uintptr_t addr) -> T
    {
        T val{};
        if (hProcess && addr) {
            ReadProcessMemory(hProcess, (LPCVOID)addr, &val, sizeof(T), NULL);
        }
        return val;
    }

    template <typename T>
    auto write(uintptr_t addr, const T& val) -> bool
    {
        if (!hProcess || !addr) return false;
        return WriteProcessMemory(hProcess, (LPVOID)addr, &val, sizeof(T), NULL) != 0;
    }

    auto read_physical(PVOID addr, void* buffer, size_t size) -> bool
    {
        if (!hProcess || !addr || !buffer) return false;
        return ReadProcessMemory(hProcess, (LPCVOID)addr, buffer, size, NULL) != 0;
    }

    auto read_string(uintptr_t addr, size_t max_len = 256) -> std::string
    {
        std::string str;
        char ch = 0;
        for (size_t i = 0; i < max_len; i++) {
            ch = read<char>(addr + i);
            if (ch == '\0') break;
            str.push_back(ch);
        }
        return str;
    }

    auto unitialize_driver() -> void
    {
        if (hProcess) {
            CloseHandle(hProcess);
            hProcess = NULL;
        }
    }
};
