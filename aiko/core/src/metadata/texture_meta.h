#pragma once

#include "aiko_types.h"
#include "assets/asset_id.h"

#include "types/texture_types.h"

namespace aiko
{

    struct TextureDesc
    {
        TextureType type = TextureType::INVALID;
        TextureFormat format = TextureFormat::INVALID;
        int width = 0;
        int height = 0;
        int mipmaps = 1;
        bool computeWrite = false;
    };

    struct TextureInfo
    {
        TextureType type = TextureType::INVALID;
        TextureFormat format = TextureFormat::INVALID;
        int width = 0;
        int height = 0;
        int mipmaps = 1;
        bool computeWrite = false;
        bool valid = false;
    };

    struct SamplerDesc
    {
        TextureFilter minFilter = TextureFilter::Linear;
        TextureFilter magFilter = TextureFilter::Linear;
        TextureMipFilter mipFilter = TextureMipFilter::None;
        TextureWrapMode wrapU = TextureWrapMode::Repeat;
        TextureWrapMode wrapV = TextureWrapMode::Repeat;
    };

}