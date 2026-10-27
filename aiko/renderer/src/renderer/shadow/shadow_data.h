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

        bool isValid() const
        {
            return depthTexture != nullptr;
        }
    };
}
