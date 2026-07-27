#pragma once
#include <cstdint>

namespace offsets
{
	constexpr uintptr_t view_render = 0x3f30868;
	constexpr uintptr_t view_matrix = 0x11a390;

	constexpr uintptr_t local_player = 0x26abff8;
	constexpr uintptr_t entity_list = 0x641a428;

	constexpr uintptr_t health = 0x320;
	constexpr uintptr_t max_health = 0x470;
	
	constexpr uintptr_t sheild = 0x190;
	constexpr uintptr_t max_sheild = 0x194;

	constexpr uintptr_t team_id = 0x330;

	constexpr uintptr_t life_state = 0x698;
	constexpr uintptr_t bleedout_state = 0x27d8;

	constexpr uintptr_t bone_array = (0xdc8 + 0x48);

	constexpr uintptr_t last_active = 0x1a84;
	constexpr uintptr_t skin_id = 0xd78;

	constexpr uintptr_t view_model = 0x1458;
	constexpr uintptr_t model_name = 0x1e7f9c0;
}