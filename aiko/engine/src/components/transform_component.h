#pragma once

#include <aiko_types.h>
#include <core/transform.h>

#include "models/component.h"

namespace aiko
{

    class TransforComponent : public Component
    {
    public:
        TransforComponent();
        virtual ~TransforComponent() = default;
        Transform transform;

    };

}
