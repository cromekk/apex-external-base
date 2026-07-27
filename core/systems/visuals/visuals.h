#pragma once
#include <core/rendering/render_utils.h>
#include <vector>

namespace settings
{
    extern bool team_check;
    extern bool menu_key;
    extern bool box;
}

extern auto draw_visuals(std::vector<RenderData> data, Camera& camera) -> void;