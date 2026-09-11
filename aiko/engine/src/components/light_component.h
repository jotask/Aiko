#pragma once

#include "models/component.h"
#include "models/light.h"
#include "types/render_types.h"

namespace aiko
{
    class LightComponent : public Component
    {
    public:
        LightComponent();
        virtual ~LightComponent() = default;

        void setPrimitive(LightType);

        Color color;
        float intensity;

        LightType type = LightType::Point;
        vec3 direction = vec3(0.0f, -1.0f, 0.0f);
        float range = 10.0f;
        float innerCos = 0.8f;
        float outerCos = 0.6f;

        void setDirectional(Color color, vec3 dir, float range);
        void setPointLight(Color color, float range);
        void setSpot(Color color, vec3 dir, float inned, float outer);

    };

}
