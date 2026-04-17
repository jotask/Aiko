#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>

#include "aiko_shape_desc.h"

namespace aiko::physics
{
    JPH::RefConst<JPH::Shape> createJoltShape(const ShapeDesc& desc);
}