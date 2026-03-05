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

    enum class TextureFilter
    {
        Nearest,
        Linear,
    };

    enum class TextureMipFilter
    {
        None,
        Nearest,
        Linear,
    };

    enum class TextureWrapMode
    {
        Repeat,
        Clamp,
        Mirror,
    };

    struct  Texture
    {
        TextureFilter   minFilter               = TextureFilter::Linear;
        TextureFilter   magFilter               = TextureFilter::Linear;
        TextureMipFilter mipFilter              = TextureMipFilter::Linear;
        TextureWrapMode wrapU                   = TextureWrapMode::Repeat;
        TextureWrapMode wrapV                   = TextureWrapMode::Repeat;
        uint8_t anisotropy                      = 1;
        TextureType     type                    = TextureType::INVALID;
        TextureFormat   format                  = TextureFormat::INVALID;
        int             width;                  // Texture base width
        int             height;                 // Texture base height
        int             mipmaps;                // Mipmap levels, 1 by default
        bool            computeWrite            = false; // allow use this texture as compute shader output
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

