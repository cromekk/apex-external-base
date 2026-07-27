#pragma once
#define _SILENCE_CXX20_OLD_SHARED_PTR_ATOMIC_SUPPORT_DEPRECATION_WARNING 
#include <memory>
#include <engine/game/game.h>

class GameCache
{
public:
	GameCache()
	{
		game = std::make_shared<Game>();
		update_instance = std::make_shared<Game>(*game);
		published_instance = std::make_shared<Game>(*game);
		std::atomic_store_explicit(&pub_game, published_instance, std::memory_order_release);
	}

	auto tick() noexcept -> void;
	auto stop() noexcept -> void { running.store(false, std::memory_order_release); }

	[[nodiscard]] auto get_game_copy() const -> std::shared_ptr<Game> {
		return std::atomic_load_explicit(&pub_game, std::memory_order_acquire);
	}

private:
	std::shared_ptr<Game> game;
	std::shared_ptr<Game> update_instance;
	std::shared_ptr<Game> published_instance;
	std::shared_ptr<Game> pub_game;
	std::atomic<bool> running{ true };
};