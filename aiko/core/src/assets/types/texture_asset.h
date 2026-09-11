#pragma once

#include <types/color.h>

#include "metadata/texture_meta.h"

namespace aiko
{

    struct TextureAsset
    {
        TextureDesc desc;
        vector<Color> pixels;
    };

}
