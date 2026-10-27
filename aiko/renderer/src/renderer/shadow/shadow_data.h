#pragma once

#include "models/texture.h"

#include <math/math.h>

namespace aiko
{
    class Texture;
}

namespace aiko::renderer
{
    enum class ShadowMapType
    {
        Projected,
        Point
    };

    struct ShadowData
    {
        mat4 lightViewProjection{1.0f};
        const Texture* depthTexture = nullptr;

        u32 lightIndex = 0;

        ShadowMapType type = ShadowMapType::Projected;

        float nearPlane = 0.1f;
        float farPlane = 1.0f;

        bool isValid() const
        {
            return depthTexture != nullptr && depthTexture->isValid();
        }
    };
}
