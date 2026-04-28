#pragma once

#include "assets/types/mesh_asset.h"
#include "models/mesh.h"

namespace aiko
{
    struct PhysicsDebugProxy
    {
        bool enabled = false;
        bool built = false;
        Color color = MAGENTA;
        Mesh mesh{};
    };
}