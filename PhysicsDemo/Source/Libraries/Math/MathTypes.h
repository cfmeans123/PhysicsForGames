#pragma once

#include "MathDebug.h"
#include "Platform/PlatformCore.h"

#define GLM_FORCE_SILENT_WARNINGS

#include "glm/glm.hpp"

namespace jm::math
{
	using vector2_f32 = glm::f32vec2;
	using vector3_f32 = glm::f32vec3;
	using vector4_f32 = glm::f32vec4;

	using matrix22_f32 = glm::f32mat2x2;
	using matrix33_f32 = glm::f32mat3x3;
	using matrix44_f32 = glm::f32mat4x4;

	using colour3_f32 = vector3_f32;

	template<class T>
	concept scalar_type = std::is_arithmetic_v<T>;

	template <scalar_type A>
	using vector2 = glm::tvec2<A>;
	template <scalar_type A>
	using vector3 = glm::tvec3<A>;
	template <scalar_type A>
	using vector4 = glm::tvec4<A>;
	template <size_t N, scalar_type A>
	using vectorN = glm::vec<N, A>;

	template <typename T>
	using matrix22 = glm::tmat2x2<T>;
	template <typename T>
	using matrix33 = glm::tmat3x3<T>;
	template <typename T>
	using matrix44 = glm::tmat4x4<T>;

#define JM_DefineArithmeticScalarConstant(name, fxn) \
	template <typename T> \
	constexpr T name() { fxn; }

	JM_DefineArithmeticScalarConstant(zero, if (std::is_same_v<T, bool>) { return false; }
	else { return T(0); });

	JM_DefineArithmeticScalarConstant(one, if (std::is_same_v<T, bool>) { return true; }
	else { return T(1); });

	JM_DefineArithmeticScalarConstant(infinity, return std::numeric_limits<T>::infinity());

	JM_DefineArithmeticScalarConstant(epsilon, return std::numeric_limits<T>::epsilon());

	JM_DefineArithmeticScalarConstant(pi, return T(3.141592653589793238462643383279503l));

	JM_DefineArithmeticScalarConstant(two_pi, return T(6.283185307179586476925286766559006l));

	JM_DefineArithmeticScalarConstant(pi_over_two, return T(1.570796326794896619231321691639751l));

	constexpr colour3_f32 red = { 1.0f, 0.0f, 0.0f };
	constexpr colour3_f32 yellow = { 1.0f, 1.0f, 0.0f };
	constexpr colour3_f32 green = { 0.0f, 1.0f, 0.0f };
	constexpr colour3_f32 cyan = { 0.0f, 1.0f, 1.0f };
	constexpr colour3_f32 blue = { 0.0f, 0.0f, 1.0f };
	constexpr colour3_f32 magenta = { 1.0f, 0.0f, 1.0f };
	constexpr colour3_f32 white = { 1.0f, 1.0f, 1.0f };
	constexpr colour3_f32 gray = { 0.5f, 0.5f, 0.5f };
	constexpr colour3_f32 black = { 0.0f, 0.0f, 0.0f };

	constexpr vector2_f32 zero2{ 0.f };
	constexpr vector3_f32 zero3{ 0.f };

	constexpr f32 epsilon_f32 = math::epsilon<f32>();

	using glm::length;
	using glm::radians;
}
