#pragma once

#include <aiko_types.h>
#include <core/transform.h>

#include "models/component.h"

namespace aiko
{

    class TransformComponent : public Component
    {
    public:
        TransformComponent();
        virtual ~TransformComponent() = default;
        Transform transform;

    };

}
