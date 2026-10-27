#pragma once

#include "types/texture_types.h"

namespace aiko
{

    struct FrameBufferDesc
    {
        int width = 0;
        int height = 0;

        TextureFormat colorFormat = TextureFormat::RGBA8;
        TextureFormat depthFormat = TextureFormat::INVALID;
    };

}
