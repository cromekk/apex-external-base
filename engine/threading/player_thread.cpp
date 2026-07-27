#define _SILENCE_CXX20_OLD_SHARED_PTR_ATOMIC_SUPPORT_DEPRECATION_WARNING 
#include "player_thread.h"
#include <core/systems/visuals/visuals.h>
#include <include/includes.h>
#include <unordered_map>
#include <unordered_set>

#undef min
#undef max
auto PlayerCache::tick() noexcept -> void
{
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
    using clock = std::chrono::steady_clock;
    auto target_interval = std::chrono::milliseconds(15);
    std::unordered_map<uintptr_t, std::shared_ptr<Player>> player_map;
    std::mutex player_map_mutex;

    while (running.load(std::memory_order_acquire))
    {
        auto start = clock::now();
        auto game_snapshot = get_world()->get_game_copy();
        if (!game_snapshot) {
            std::this_thread::sleep_for(target_interval);
            continue;
        }

        auto entity_list = game_snapshot->get_entity_list();
        if (entity_list.empty()) {
            std::this_thread::sleep_for(target_interval);
            continue;
        }

        if (!local_player->update(recode::driver.read<uintptr_t>(recode::process_image + 0x282C358)))  {
            std::this_thread::sleep_for(target_interval);
            continue;
        }

        auto temp_players = std::make_shared<std::vector<std::shared_ptr<Player>>>();
        const size_t max_threads = std::thread::hardware_concurrency();
        const size_t thread_count = std::min(max_threads, std::max(size_t(1), entity_list.size()));
        const size_t entity_chunk = (entity_list.size() + thread_count - 1) / thread_count;
        std::vector<std::vector<std::pair<uintptr_t, std::shared_ptr<Player>>>> thread_results(thread_count);
        std::vector<std::thread> process_threads;

        for (size_t t = 0; t < thread_count; ++t)
        {
            process_threads.emplace_back([&, t]()
                {
                    size_t start_idx = t * entity_chunk;
                    if (start_idx >= entity_list.size()) return;
                    size_t end_idx = std::min(start_idx + entity_chunk, entity_list.size());

                    for (size_t i = start_idx; i < end_idx; ++i)
                    {
                        uintptr_t pawn = entity_list[i];

                        std::shared_ptr<Player> player;
                        {
                            std::lock_guard<std::mutex> lock(player_map_mutex);
                            auto it = player_map.find(pawn);
                            if (it != player_map.end())
                                player = it->second;
                        }
                        if (!player)
                            player = std::make_shared<Player>();

                        if (player->update(pawn)) {
							if (player->get_team_id() != local_player->get_team_id() || !settings::team_check)
                                thread_results[t].emplace_back(pawn, player);
                        }
                    }
                });
        }

        for (auto& th : process_threads) th.join();

        std::unordered_map<uintptr_t, std::shared_ptr<Player>> next_frame;
        for (auto& results : thread_results) {
            for (auto& [pawn, player] : results) {
                temp_players->push_back(player);
                next_frame[pawn] = player;
            }
        }

        player_map = std::move(next_frame);

        if (!temp_players->empty())
            std::atomic_store_explicit(&pub_players, temp_players, std::memory_order_release);

        auto total = std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - start);
        if (total < target_interval)
            std::this_thread::sleep_for(target_interval - total);
    }
}