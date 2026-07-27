#pragma once
#include <deps/imgui/imgui.h>
#include <core/rendering/render_utils.h>

extern auto draw_cornered_box(float x, float y, float w, float h, const ImColor color, float thickness, ImDrawList* draw) -> void;