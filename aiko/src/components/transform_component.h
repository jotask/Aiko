#pragma once

#include "aiko_types.h"
#include "models/component.h"
#include "shared/math.h"

namespace aiko
{

    class Transform : public Component
    {
    public:
        Transform();
        virtual ~Transform() = default;

        vec3 position = vec3(0.0f);
        vec3 rotation = vec3(0.0f);
        vec3 scale = vec3(1.0f);

        mat4 getMatrix();

    };

}
