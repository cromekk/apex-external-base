#include "game_thread.h"
#include <include/includes.h>

auto GameCache::tick() noexcept -> void
{
	using clock = std::chrono::steady_clock;
	constexpr auto target_interval = std::chrono::milliseconds(150);

	while (running.load(std::memory_order_acquire))
	{
		auto start = clock::now();

		if (!update_instance->update())
		{
			auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - start);

			if (elapsed < target_interval)
				std::this_thread::sleep_for(target_interval - elapsed);

			continue;
		}

		std::atomic_store_explicit(&pub_game, update_instance, std::memory_order_release);
		std::swap(update_instance, published_instance);

		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - start);

		if (elapsed < target_interval)
			std::this_thread::sleep_for(target_interval - elapsed);
	}
}