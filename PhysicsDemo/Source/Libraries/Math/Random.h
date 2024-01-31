#pragma once

#include "MathTypes.h"

#include <random>

namespace jm::math::random
{
	template <typename T>
	constexpr bool is_bool = std::is_same_v<T, bool>;


	u64 get_thread_seed();

	class core
	{
		std::mt19937_64 rng;
		uint64_t timeSeed;
		std::seed_seq ss;

	public:

		core();

		explicit core(u64 seed);

		std::mt19937_64& engine();
	};

	template <typename T>
	class uniform_generator
	{
		static_assert(!is_bool<T>, "Please use boolean_generator instead!");
		core core_generator;
		std::conditional_t<std::is_floating_point_v<T>, std::uniform_real_distribution<T>, std::uniform_int_distribution<T>> uniform;

	public:

		//float type -> [min, max), int type -> [min, max]
		uniform_generator(u64 seed, T min, T max)
			: core_generator(seed)
			, uniform(min, max)
		{
		}

		T operator()(void)
		{
			return uniform(core_generator.engine());
		}
	};

	class boolean_generator
	{
		core core_generator;
		std::bernoulli_distribution bernoulli;

	public:

		boolean_generator(u64 seed)
			: core_generator(seed)
			, bernoulli()
		{
		}

		bool operator()(void)
		{
			return bernoulli(core_generator.engine());
		}
	};

	bool flip();

	//[0, 1)
	template <typename T>
	T unit()
	{
		static_assert(!is_bool<T>, "Use flip for Boolean type instead!");
		static thread_local uniform_generator<T> rng(get_thread_seed(), math::zero<T>(), math::one<T>());
		return rng();
	}

	//[0, 2pi)
	template <typename T>
	inline T angle()
	{
		return math::two_pi<T>() * unit<T>();
	}

	//[a, b)
	template <typename T>
	T scalar(T min, T max)
	{
		JM_MATH_ASSERT(min < max);
		return (max - min) * unit<T>() + min;
	}

	template <typename T>
	inline vector2<T> unit_circle()
	{
		const T theta = angle<T>();
		return vector2<T>{ cos(theta), sin(theta) };
	}

	template <typename T>
	inline vector2<T> unit_disk()
	{
		return std::sqrt(unit<T>()) * unit_circle<T>();
	}

	template <typename T>
	inline vector3<T> unit_sphere()
	{
		const T phi = angle<T>() * 0.5f;
		return { sin(phi) * unit_circle<T>(), cos(phi) };
	}

	template <typename T>
	inline vector3<T> unit_ball()
	{
		return std::cbrt(unit<T>()) * unit_sphere<T>();
	}

	template <typename T>
	inline quaternion<T> unit_quaternion()
	{
		return math::angleAxis(math::random::angle<T>(), math::random::unit_sphere<T>());
	}
}
