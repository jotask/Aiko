#pragma once
#include "assets/types/texture_asset.h"

namespace wc
{
    class WolfMap;

    class WolfMapTexture
    {
    public:
        static aiko::TextureAsset build(const WolfMap& map);
    private:
        static aiko::Color getTileColor(uint16_t tileId);
    };

}
