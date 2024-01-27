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

        Vector3 position = Vector3(0.0f);
        Vector3 rotation = Vector3(0.0f);
        Vector3 scale = Vector3(1.0f);

    };

}
