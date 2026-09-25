#pragma once

#include <optional>

#include <math/math.h>
#include <types/color.h>
#include <assets/asset_id.h>

namespace aiko
{

    struct UIRect
    {
        vec2 position{0.0f};
        vec2 size{0.0f};
    };

    struct UIDrawCommand
    {
        uint32_t indexOffset = 0;
        uint32_t indexCount = 0;

        AssetId textureId = InvalidAssetId;

        UIRect rect;

        float cornerRadius = 0.0f;
        float borderThickness = 0.0f;
        Color borderColor = WHITE;

        std::optional<UIRect> clipRect;
    };

    struct UIVertex
    {
        vec2 position{0.0f};
        vec2 uv{0.0f};
        Color color = WHITE;
    };

}
