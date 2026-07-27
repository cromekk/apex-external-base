#pragma once
#include <deps/math/structs.h>
#include <engine/player/player.h>
#include <engine/camera/camera.h>
#include <windows.h>
#include <array>
#include <string>

struct RenderData
{
    float box_x, box_y;
    float box_width, box_height;
    bool valid;
};

extern auto is_on_screen(const vec2& pos, Camera& camera, int margin = 100) -> bool;
extern auto get_player_data(Player* player, Camera& camera) -> RenderData;