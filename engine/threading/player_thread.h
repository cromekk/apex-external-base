#pragma once
#include <memory>
#include <vector>
#include <engine/player/player.h>
#include <engine/threading/game_thread.h>

class PlayerCache
{
public:
    explicit PlayerCache(std::shared_ptr<GameCache> wc)
    {
        world_cache = std::move(wc);
        local_player = std::make_shared<Player>();
        pub_players = std::make_shared<std::vector<std::shared_ptr<Player>>>();
    }

    ~PlayerCache() { stop(); }

    void tick() noexcept;
    void stop() noexcept { running.store(false, std::memory_order_release); }

    [[nodiscard]] std::shared_ptr<std::vector<std::shared_ptr<Player>>> get_players() const
    {
        return std::atomic_load_explicit(&pub_players, std::memory_order_acquire);
    }

    [[nodiscard]] std::shared_ptr<Player> get_local_player() const
    {
        return std::atomic_load_explicit(&local_player, std::memory_order_acquire);
    }

    [[nodiscard]] std::shared_ptr<GameCache> get_world() const
    {
        return std::atomic_load_explicit(&world_cache, std::memory_order_acquire);
    }

private:
    std::shared_ptr<GameCache> world_cache;
    std::shared_ptr<Player> local_player;
    std::shared_ptr<std::vector<std::shared_ptr<Player>>> pub_players;
    std::atomic<bool> running{ true };
};