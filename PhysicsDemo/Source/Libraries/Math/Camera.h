#pragma once

#include "MathTypes.h"

namespace jm::math
{
	template <typename T>
	class camera3
	{
		vector3<T> lower_left_corner;
		vector3<T> horizontal;
		vector3<T> vertical;
		vector3<T> origin;

		vector3<T> back;
		vector3<T> right;
		vector3<T> up;
		T vert_fov;
		T aspect;
		T focal_distance;
		T half_height;
		T half_width;

	public:

		camera3(const vector3<T>& look_from, const vector3<T>& look_at
			, T vertical_FOV, T aspect_ratio);

		camera3(const vector3<T>& look_from, const vector3<T>& look_at
			, T vertical_FOV, T aspect_ratio, T focus_distance);

		void translate(const vector3<T>& displacement);
		void rotate(T deltaYaw, T deltaPitch);

		vector2<T> get_screen_to_world(vector2<T> const& screen_coordinates) const;

		matrix33<T> get_camera_rotation() const;
		matrix33<T> get_view_rotation() const;
		matrix44<T> get_view_transform() const;

		matrix44<T> get_perspective_transform() const;
		matrix44<T> get_orthogonal_transform() const;

		vector3<T> get_forward() const { return -back; }
		vector3<T> get_back() const { return back; }
		vector3<T> get_left() const { return -right; }
		vector3<T> get_right() const { return right; }
		vector3<T> get_up() const { return up; }
		vector3<T> get_down() const { return -up; }

		vector3<T> get_origin() const { return origin + back; }
	};

	template <typename T>
	camera3<T>::camera3(const vector3<T>& look_from, const vector3<T>& look_at
		, T vertical_FOV, T aspect_ratio)
		: camera3(look_from, look_at, vertical_FOV
			, aspect_ratio, (T)(look_from - look_at).length())
	{}

	template <typename T>
	camera3<T>::camera3(const vector3<T>& look_from, const vector3<T>& look_at
		, T vertical_FOV, T aspect_ratio, T focus_distance)
	{
		vert_fov = radians(vertical_FOV);
		aspect = aspect_ratio;
		focal_distance = focus_distance;

		back = normalize(look_from - look_at);
		right = normalize(cross({ 0.0f, 1.0f, 0.0f }, back));
		up = cross(back, right);

		origin = look_from;

		half_height = std::tanf(vert_fov * 0.5f);
		half_width = aspect * half_height;

		lower_left_corner = origin - focal_distance * half_width * right - focal_distance * half_height * up - focal_distance * back;
		horizontal = 2.0f * focal_distance * half_width * right;
		vertical = 2.0f * focal_distance * half_height * up;
	}

	template <typename T>
	void camera3<T>::translate(const vector3<T>& displacement)
	{
		origin += displacement;

		lower_left_corner = origin - focal_distance * half_width * right - focal_distance * half_height * up - focal_distance * back;
	}

	template <typename T>
	void camera3<T>::rotate(T deltaYaw, T deltaPitch)
	{
		const vector3<T> forward = -back;
		const T yaw = atan2f(forward.z, forward.x) + deltaYaw;
		T pitch = asinf(forward.y) + deltaPitch;

		const T pitchMax = math::pi_over_two<T>() - radians(1.0f);

		if (pitch > pitchMax) pitch = pitchMax;
		if (pitch < -pitchMax) pitch = -pitchMax;

		back = -vector3<T>{ cos(yaw) * cos(pitch), sin(pitch), sin(yaw) * cos(pitch) };
		right = normalize(cross({ 0.0f, 1.0f, 0.0f }, back));
		up = cross(back, right);

		lower_left_corner = origin - focal_distance * half_width * right - focal_distance * half_height * up - focal_distance * back;
		horizontal = 2.0f * focal_distance * half_width * right;
		vertical = 2.0f * focal_distance * half_height * up;

	}

	template <typename T>
	vector2<T> camera3<T>::get_screen_to_world(vector2<T> const& screen_coordinates) const
	{
		return { screen_coordinates.x * focal_distance * half_width, screen_coordinates.y * focal_distance * half_height };
	}

	template <typename T>
	matrix33<T> camera3<T>::get_camera_rotation() const
	{
		return { right, up, back };
	}

	template <typename T>
	matrix33<T> camera3<T>::get_view_rotation() const
	{
		matrix33<T> invRotation{
			{right.x, up.x, back.x},
			{right.y, up.y, back.y},
			{right.z, up.z, back.z} };
		return invRotation;
	}

	template <typename T>
	matrix44<T> camera3<T>::get_view_transform() const
	{
		return matrix44<T>(get_view_rotation()) * translation_matrix3(-origin);
	}

	template <typename T>
	matrix44<T> camera3<T>::get_perspective_transform() const
	{
		return perspective_matrix(vert_fov, aspect, 0.1f);
	}

	template <typename T>
	matrix44<T> camera3<T>::get_orthogonal_transform() const
	{
		vector2<T> farHalfSize = focal_distance * vector2<T>{ half_width, half_height };
		return orthogonal_matrix(-farHalfSize.x, farHalfSize.x, -farHalfSize.y, farHalfSize.y, 0.001f, 1000.0f);
	}
}
