#pragma once

#include "assets/types/texture_asset.h"
#include "wolf_vga_graphics_types.h"
#include "wolf_vswap_types.h"

namespace wc
{
    class WolfAssetsDecoder final
    {
    public:
        static aiko::TextureAsset decode(const WolfWallTexture& wall);
        static aiko::TextureAsset decode(const WolfSprite& sprite);
        static aiko::TextureAsset decode(const WolfVgaPicture& picture);
    };
}
