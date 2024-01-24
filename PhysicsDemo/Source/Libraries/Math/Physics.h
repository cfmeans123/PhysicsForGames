#pragma once

#include "MathTypes.h"

namespace jm::math
{
	template <typename T>
	struct rigid_motion2
	{
		vector2<T> position{};
		T rotation{};
	};

	template <typename T>
	struct rigid_motion3
	{
		vector3<T> position{};
	};
}
