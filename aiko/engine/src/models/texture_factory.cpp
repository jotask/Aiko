#include "texture_factory.h"

namespace aiko
{
    namespace texture::factory
    {
        TextureAsset generateBlank(uint width, uint height, Color color)
        {
            AIKO_ASSERT(width > 0 && height > 0, "Texture size must be greater than zero");

            TextureAsset asset{};

            asset.desc.type = TextureType::Sampled;
            asset.desc.format = TextureFormat::RGBA8;
            asset.desc.width = width;
            asset.desc.height = height;
            asset.desc.mipmaps = 1;
            asset.desc.computeWrite = false;

            asset.pixels.resize(
                static_cast<size_t>(width) * static_cast<size_t>(height),
                color);

            return asset;
        }
    }
}
