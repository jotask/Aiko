#pragma once

#include <aiko_types.h>

namespace aiko::texture
{

    enum class TextureFormat
    {
        INVALID,
        BGRA8,
        RGBA8,
        D24S8,
    };

    enum class TextureType
    {
        INVALID,
        Sampled,
        RenderTarget,
        DepthStencil
    };

    struct  Texture
    {
        TextureType      type                   = TextureType::INVALID;
        TextureFormat   format                  = TextureFormat::INVALID;
        int             width;                  // Texture base width
        int             height;                 // Texture base height
        int             mipmaps;                // Mipmap levels, 1 by default
    };

    inline int getChannelCount(TextureFormat format)
    {
        switch (format)
        {
        case TextureFormat::RGBA8:
        case TextureFormat::BGRA8:
            return 4;

        case TextureFormat::D24S8:
            return 2;

        default:
            return 0;
        }
    }

}

