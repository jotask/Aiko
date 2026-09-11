#pragma once

#include "assets/types/texture_asset.h"

namespace aiko
{
    namespace texture::factory
    {
        TextureAsset generateBlank(uint width, uint height, Color color = RAYWHITE);
    }
}
