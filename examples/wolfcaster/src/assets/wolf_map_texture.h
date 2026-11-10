#pragma once
#include "assets/types/texture_asset.h"
#include "map/wolf_map_types.h"

namespace wc
{
    class WolfMap;

    class WolfMapTexture
    {
    public:
        static aiko::TextureAsset build(const WolfMap& map);
    private:
        static aiko::Color getTileColor(TileId tileId);
    };

}
