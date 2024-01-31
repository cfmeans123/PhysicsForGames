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
}