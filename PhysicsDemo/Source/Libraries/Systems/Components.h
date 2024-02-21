#pragma once

#include "Math/Physics.h"

namespace jm
{
    using spatial2_component = math::rigid_motion2<f32>;

    using spatial3_component = math::rigid_motion3<f32>;

    enum class shape_component
    {
        Box,
        Sphere
    };

    using linear_body2_component = math::linear_body2<f32>;
    using linear_body3_component = math::linear_body3<f32>;
}