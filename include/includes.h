#pragma once
#define _SILENCE_CXX20_OLD_SHARED_PTR_ATOMIC_SUPPORT_DEPRECATION_WARNING
#include <Windows.h>
#include <iostream>
#include <mutex>
#include <atomic>
#include <thread>
#include <string>
#include <memory>
#include <chrono>
#include <vector>
#include <d3dx11.h>

#include <deps/crypter/sk.h>
#include <deps/memory/mem.h>

#include <deps/imgui/imgui.h>
#include <deps/imgui/imgui_internal.h>
#include <deps/imgui/imgui_impl_dx11.h>
#include <deps/imgui/imgui_impl_win32.h>

namespace recode
{
	inline float render_dist = 360.f;
	inline std::atomic_bool is_running = false;
	inline Kmem driver;
	inline uintptr_t process_image;
}