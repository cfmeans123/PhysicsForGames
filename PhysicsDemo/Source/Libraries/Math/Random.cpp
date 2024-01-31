#include "Random.h"

#include <chrono>
#include <thread>

namespace jm::math::random
{
	u64 get_thread_seed()
	{
		return 0xfe3a7cea7219dd00 + std::hash<std::thread::id>{}(std::this_thread::get_id());
	}
	
	core::core()
		: core(std::chrono::high_resolution_clock::now().time_since_epoch().count())
	{
	}

	core::core(u64 seed)
		: rng()
		, timeSeed(seed)
		, ss{ uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed >> 32) }
	{
		rng.seed(ss);
	}

	std::mt19937_64& core::engine() { return rng; }

	bool flip()
	{
		static thread_local boolean_generator rng(get_thread_seed());
		return rng();
	}
}
