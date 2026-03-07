#pragma once

#include "aiko_types.h"
#include "models/component.h"

namespace aiko
{

    class ModelComponent : public Component
    {
    public:

        ModelComponent();
        virtual ~ModelComponent() = default;

        void setModelId(const AssetId& id) { m_modelId = id; }
        const AssetId& getModelId() const { return m_modelId; }

    private:

        AssetId m_modelId;
    };

}