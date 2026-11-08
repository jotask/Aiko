#pragma once

#include "assets/types/texture_asset.h"
#include "wolf_vswap_types.h"

namespace wc
{
    class WolfSpriteDecoder
    {
    public:
        static aiko::TextureAsset decode(const WolfSprite& sprite);
    };
}
