#pragma once

#include <vector>

#include <aiko_types.h>
#include <math/math.h>
#include <core/transform.h>

#include "models/component.h"

namespace aiko
{

    class TransforComponent : public Component
    {
    public:
        TransforComponent();
        virtual ~TransforComponent() = default;

        TransforComponent* parent;
        std::vector<TransforComponent*> childs;

        Transform transform;

    };

}
