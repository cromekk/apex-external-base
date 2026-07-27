#include "visuals.h"
#include <core/rendering/draw_primitives.h>

namespace settings
{
    bool menu_key = true;
    bool team_check = true;
    bool box = true;
}

auto draw_visuals(std::vector<RenderData> data, Camera& camera) -> void
{
    if (data.empty())
        return;

    auto* draw = ImGui::GetBackgroundDrawList();

    for (const auto& data : data)
    {
        const ImColor color = ImColor(220, 220, 220, 255);
        if (settings::box)
            draw_cornered_box(data.box_x, data.box_y, data.box_width, data.box_height, color, 1.f, draw);
    }
}