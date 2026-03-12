#pragma once

namespace aiko
{

    enum class TextureFormat
    {
        INVALID,
        BGRA8,
        RGBA8,
        D24S8,
        D32F,
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

