#pragma once

#include <aiko_types.h>

#include "models/component.h"

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

        // FIXME temporary as component now has all interfaces
        virtual void update() override { };
        virtual void render() override { };

    };

}
