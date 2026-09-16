#pragma once

#include <math/math.h>
#include <types/color.h>
#include <assets/asset_id.h>

namespace aiko
{

    struct UIDrawCommand
    {
        uint32_t indexOffset = 0;
        uint32_t indexCount = 0;

        AssetId textureId = InvalidAssetId;
    };

    struct UIRect
    {
        vec2 position{0.0f};
        vec2 size{0.0f};
    };

    struct UIVertex
    {
        vec2 position{0.0f};
        vec2 uv{0.0f};
        Color color = WHITE;
    };

}
