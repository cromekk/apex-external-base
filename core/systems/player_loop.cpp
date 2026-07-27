#include "player_loop.h"
#include <core/rendering/render_utils.h>
#include <core/rendering/draw_primitives.h>
#include <core/systems/visuals/visuals.h>
#include <vector>
#include <iostream>
#include <include/includes.h>

auto player_loop(std::shared_ptr<PlayerCache> cache) -> void
{
    if (!cache)
        return;

    Camera cam{};
    if (!cam.update())
        return;

    const auto& players = cache->get_players();
    if (players->empty())
        return;

    std::vector<RenderData> draw_data;
    draw_data.reserve(players->size());

    for (auto& p : *players)
    {
        auto data = get_player_data(p.get(), cam);
        if (!data.valid)
            continue;

        draw_data.push_back(data);
    }

    draw_visuals(draw_data, cam);
}