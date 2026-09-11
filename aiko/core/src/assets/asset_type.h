#pragma once

#include "aiko_types.h"

namespace aiko
{

    enum class AssetType : u8
    {
        Unknown = 0,
        Texture,
        Mesh,
        Model,
        Shader,
        ComputeShader,
        Material
    };

}
