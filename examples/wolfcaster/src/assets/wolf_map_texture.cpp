#include "wolf_map_texture.h"

#include "map/wolf_map.h"

namespace wc
{

    aiko::TextureAsset WolfMapTexture::build(const WolfMap& map)
    {

        aiko::TextureAsset asset;

        asset.desc.type = aiko::TextureType::Sampled;
        asset.desc.format = aiko::TextureFormat::RGBA8;
        asset.desc.width = map.getWidth();
        asset.desc.height = map.getHeight();
        asset.desc.mipmaps = 1;
        asset.desc.computeWrite = false;

        asset.pixels.resize(size_t(map.getWidth()) * size_t(map.getHeight()));

        for (size_t y = 0 ; y < map.getHeight(); ++y)
        {
            for (size_t x = 0 ; x < map.getWidth(); ++x)
            {
                const TileId tileMap = map.getGeometryTile(x, y);
                const aiko::Color pixelColor = getTileColor(tileMap);
                asset.pixels[y * size_t(map.getWidth()) + size_t(x)] = pixelColor;
            }
        }

        return asset;

    }

    aiko::Color WolfMapTexture::getTileColor(TileId tileId)
    {
        if (tileId >= 1 && tileId <= 63) // Wall
        {
            return aiko::Color(100 / 255.0f, 116 / 255.0f, 138 / 255.0f );
        }
        if (tileId >= 90 && tileId <= 101) // Door
        {
            return aiko::Color(218 / 255.0f, 167 / 255.0f, 76 / 255.0f );
        }
        return aiko::Color(35 / 255.0f, 24 / 255.0f, 61 / 255.0f );
    }
}
