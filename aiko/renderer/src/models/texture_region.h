#pragma once

#include <aiko_types.h>

#include "math/math_vector.h"

namespace aiko
{

    struct TextureRegion
    {
        vec2 min = {0.0f, 0.0f};
        vec2 max = {1.0f, 1.0f};

        static TextureRegion full()
        {
            return {};
        }

        static TextureRegion fromPixels(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t textureWidth, uint32_t textureHeight)
        {
            AIKO_ASSERT(textureWidth > 0, "Texture region requires a non-zero texture width");
            AIKO_ASSERT(textureHeight > 0, "Texture region requires a non-zero texture height");
            AIKO_ASSERT(width > 0, "Texture region requires a non-zero width");
            AIKO_ASSERT(height > 0, "Texture region requires a non-zero height");
            AIKO_ASSERT(x <= textureWidth, "Texture region x exceeds texture width");
            AIKO_ASSERT(y <= textureHeight, "Texture region y exceeds texture height");
            AIKO_ASSERT(width <= textureWidth - x, "Texture region exceeds texture width");
            AIKO_ASSERT(height <= textureHeight - y, "Texture region exceeds texture height");

            const vec2 textureSize =
            {
                static_cast<float>(textureWidth),
                static_cast<float>(textureHeight)
            };

            return
            {
                .min =
                {
                    static_cast<float>(x) / textureSize.x,
                    static_cast<float>(y) / textureSize.y
                },
                .max =
                {
                    static_cast<float>(x + width) / textureSize.x,
                    static_cast<float>(y + height) / textureSize.y
                }
            };
        }
    };

}
