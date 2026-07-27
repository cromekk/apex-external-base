#pragma once
#include "draw_primitives.h"

auto draw_cornered_box(float x, float y, float w, float h, const ImColor color, float thickness, ImDrawList* draw) -> void
{
    draw->AddLine(ImVec2(x, y), ImVec2(x, y + (h / 3)), color, thickness);
    draw->AddLine(ImVec2(x, y), ImVec2(x + (w / 3), y), color, thickness);
    draw->AddLine(ImVec2(x + w - (w / 3), y), ImVec2(x + w, y), color, thickness);
    draw->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + (h / 3)), color, thickness);
    draw->AddLine(ImVec2(x, y + h - (h / 3)), ImVec2(x, y + h), color, thickness);
    draw->AddLine(ImVec2(x, y + h), ImVec2(x + (w / 3), y + h), color, thickness);
    draw->AddLine(ImVec2(x + w - (w / 3), y + h), ImVec2(x + w, y + h), color, thickness);
    draw->AddLine(ImVec2(x + w, y + h - (h / 3)), ImVec2(x + w, y + h), color, thickness);
}
