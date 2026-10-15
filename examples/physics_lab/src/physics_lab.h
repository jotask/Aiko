#pragma once

#include "assets/asset_id.h"
#include "layers/layer.h"

#include <aiko_body_desc.h>

namespace aiko
{
    class GameObject;
}

namespace aiko::lab
{
    class PhysicsLab final : public Layer
    {
    public:
        void init() override;

    private:
        GameObject* addBody(const char* name, physics::BodyDesc desc, GameObject* parent = nullptr);

        AssetId m_modelShader = InvalidAssetId;
    };
}