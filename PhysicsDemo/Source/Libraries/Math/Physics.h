#pragma once

#include "MathTypes.h"

namespace jm::math
{
	template <typename T>
	struct rigid_motion2
	{
		vector2<T> position{};
		T orientation{};
	};

	template <typename T>
	struct rigid_motion3
	{
		vector3<T> position{};
		quaternion<T> orientation{};
	};

	template <typename T>
	struct linear_body2
	{
		linear_body2(vector2<T> const& v0, T mass)
			: velocity(v0)
			, mass(mass)
			, inverse_mass(T(1) / mass)
		{}
		vector2<T> applied_force{};
		vector2<T> velocity;
		const T mass;
		const T inverse_mass;
	};

	template <typename T>
	struct linear_body3
	{
		linear_body3(vector3<T> const& v0, T mass)
			: velocity(v0)
			, mass(mass)
			, inverse_mass(T(1) / mass)
		{}
		vector3<T> applied_force{};
		vector3<T> velocity;
		const T mass;
		const T inverse_mass;
	};

	template <typename T>
	struct rotational_body2
	{
		rotational_body2(T omega0, T inertia)
			: velocity(omega0)
			, inertia(inertia)
			, inverse_inertia(T(1) / inertia)
		{}
		vector2<T> applied_torque{};
		vector2<T> velocity;
		const T inertia;
		const T inverse_inertia;
	};

	template <typename T>
	struct rotational_body3
	{
		rotational_body3(vector3<T> const& omega0, vector3<T> const& inertia)
			: velocity(omega0)
			, inertia(inertia)
			, inverse_inertia(T(1) / inertia)
		{}
		vector3<T> applied_torque{};
		vector3<T> velocity;
		const vector3<T> inertia; //Assumes symmetric shapes
		const vector3<T> inverse_inertia;
	};

	template <typename T, typename V>
	inline void euler_integration(V& value, V const& derivative, T delta_t)
	{
		value += (delta_t * derivative);
	}

	template <typename T, typename V>
	inline void trapezoidal_integration(V& value, V const& derivative, V const& last_derivative, T delta_t)
	{
		value += (delta_t * (last_derivative + derivative) * 0.5f);
	}
}
