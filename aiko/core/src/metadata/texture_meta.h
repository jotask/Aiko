#pragma once

#include "aiko_types.h"
#include "assets/asset_id.h"

#include "types/texture_types.h"

namespace aiko
{

    struct TextureSubresource
    {
        u32 mipLevel = 0;
        u32 arrayLayer = 0;
    };

    struct TextureDesc
    {
        TextureType type = TextureType::INVALID;
        TextureFormat format = TextureFormat::INVALID;
        TextureDimension dimension = TextureDimension::Texture2D;
        int width = 0;
        int height = 0;
        int mipmaps = 1;
        bool computeWrite = false;
    };

    struct TextureInfo
    {
        TextureType type = TextureType::INVALID;
        TextureFormat format = TextureFormat::INVALID;
        TextureDimension dimension = TextureDimension::Texture2D;
        int width = 0;
        int height = 0;
        int mipmaps = 1;
        bool computeWrite = false;
        bool valid = false;
    };

    struct SamplerState
    {
        TextureFilter minFilter = TextureFilter::Linear;
        TextureFilter magFilter = TextureFilter::Linear;

        TextureMipFilter mipFilter = TextureMipFilter::Linear;

        TextureWrapMode wrapU = TextureWrapMode::Repeat;
        TextureWrapMode wrapV = TextureWrapMode::Repeat;

        bool operator==(const SamplerState& other) const = default;
    };

}
