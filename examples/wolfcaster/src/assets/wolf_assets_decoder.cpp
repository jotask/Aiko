#include "wolf_assets_decoder.h"

#include "wolf_palette.h"

namespace wc
{

    aiko::TextureAsset WolfAssetsDecoder::decode(const WolfWallTexture& wall)
    {
        aiko::TextureAsset asset;

        asset.desc.type = aiko::TextureType::Sampled;
        asset.desc.format = aiko::TextureFormat::RGBA8;
        asset.desc.width = 64;
        asset.desc.height = 64;
        asset.desc.mipmaps = 1;
        asset.desc.computeWrite = false;

        asset.pixels.resize(64 * 64);

        for (size_t i = 0 ; i < wall.paletteIndices.size(); ++i)
        {
            const uint8_t paletteIndex = wall.paletteIndices[i];
            const aiko::Color color = WolfPalette::getColor(paletteIndex);
            asset.pixels[i] = color;
        }

        return asset;
    }

    aiko::TextureAsset WolfAssetsDecoder::decode(const WolfSprite& sprite)
    {
        aiko::TextureAsset asset;

        asset.desc.type = aiko::TextureType::Sampled;
        asset.desc.format = aiko::TextureFormat::RGBA8;
        asset.desc.width = 64;
        asset.desc.height = 64;
        asset.desc.mipmaps = 1;
        asset.desc.computeWrite = false;

        asset.pixels.resize(64 * 64);

        for (size_t i = 0 ; i < sprite.paletteIndices.size(); ++i)
        {
            const uint8_t paletteIndex = sprite.paletteIndices[i];
            const uint8_t alphaMask = sprite.alphaMask[i];
            aiko::Color color = WolfPalette::getColor(paletteIndex);
            color.a = alphaMask / 255.0f;
            asset.pixels[i] = color;
        }

        return asset;
    }
}
