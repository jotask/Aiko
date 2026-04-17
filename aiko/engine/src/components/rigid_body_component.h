#pragma once

#include <aiko_types.h>
#include "assets/asset_id.h"
#include "assets/types/mesh_asset.h"
#include "metadata/material_instance.h"

#include "models/component.h"

namespace aiko
{

    class RigidBodyComponent : public Component
    {

    public:

        RigidBodyComponent();
        virtual ~RigidBodyComponent() = default;

        virtual void init() override;

    private:

    };

}