#pragma once

#include <math/math.h>

namespace aiko
{
    class Texture;
}

namespace aiko::renderer
{
    struct ShadowData
    {
        mat4 lightViewProjection{1.0f};
        const Texture* depthTexture = nullptr;

        u32 lightIndex = 0;

        bool isValid() const
        {
            return depthTexture != nullptr;
        }
    };
}
