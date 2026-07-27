#define _CRT_SECURE_NO_WARNINGS
#include <include/includes.h>
#include <deps/overlay/overlay.h>
#include <engine/threading/game_thread.h>
#include <engine/threading/player_thread.h>
#include <core/render_loop.h>


auto process_check() -> void
{
    while (recode::is_running)
    {
        if (!recode::driver.GetProcessPid(skCrypt(L"r5apex_dx12.exe").decrypt()))
            break;

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    atomic_store_explicit(&recode::is_running, false, std::memory_order_release);
}

auto main() -> std::int32_t
{
    system(skCrypt("color 1"));
    std::cout << skCrypt(R"(                                    
    > APEX-EXTERNAL-BASE
    > F1 IN GAME)") << std::endl;

    while (!GetAsyncKeyState(VK_F1))
        Sleep(100);

    if (!recode::driver.setup(skCrypt(L"r5apex_dx12.exe").decrypt()))
    {
        std::cout << skCrypt("    > ERROR: -1") << std::endl;
        recode::driver.unitialize_driver();
        Sleep(2000);
        return -1;
    }
    std::cout << skCrypt("    > PROCESS ID: ") << recode::driver.Pid() << std::endl;

    recode::process_image = recode::driver.GetBase();
    if (!recode::process_image)
    {
        std::cout << skCrypt("    > ERROR: -2") << std::endl;
        recode::driver.unitialize_driver();
        Sleep(2000);
        return -2;
    }
    auto cr3 = recode::driver.GetCR3();

    std::cout << skCrypt("    > CR3: 0X") << std::hex << cr3 << std::endl;
    std::cout << skCrypt("    > IMAGE: 0X") << std::hex << recode::process_image << std::endl;
    std::cout << skCrypt("    > TAB INTO GAME") << std::endl;

    bool window_focus = false;
    while (!window_focus) {
        DWORD ForegroundWindowProcessID;
        GetWindowThreadProcessId(GetForegroundWindow(), &ForegroundWindowProcessID);
        if (recode::driver.Pid() == ForegroundWindowProcessID)
            window_focus = true;
    }

    Overlay ovr{};
    if (!ovr.hijack_window())
    {
        std::cout << skCrypt("    > ERROR: -3") << std::endl;
        recode::driver.unitialize_driver();
        Sleep(2000);
        return -3;
    }

    if (!ovr.create_device())
    {
        std::cout << skCrypt("    > ERROR: -4") << std::endl;
        recode::driver.unitialize_driver();
        ovr.destroy_device();
        Sleep(2000);
        return -4;
    }
    recode::is_running.store(true, std::memory_order_release);


    auto game_cache = std::make_shared<GameCache>();
    auto player_cache = std::make_shared<PlayerCache>(game_cache);

    std::jthread process_thread([&]() { process_check(); });
    std::jthread game_thread([game_cache]() { game_cache->tick(); });
    std::jthread player_thread([player_cache]() { player_cache->tick(); });


    while (recode::is_running)
        core::render_loop(player_cache, ovr);

    std::cout << skCrypt("    > THANK YOU FOR USING APEX-EXTERNAL-BASE") << std::endl;
    game_cache->stop();
    player_cache->stop();
    recode::driver.unitialize_driver();
    ovr.destroy_device();
    return 0;

}
