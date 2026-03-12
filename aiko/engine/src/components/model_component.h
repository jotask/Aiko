#pragma once

#include "aiko_types.h"
#include "models/component.h"
#include "metadata/material_instance.h"

namespace aiko
{

    class ModelComponent : public Component
    {
    public:

        ModelComponent();
        virtual ~ModelComponent() = default;

        void setModelId(const AssetId& id) { m_modelId = id; }
        const AssetId& getModelId() const { return m_modelId; }

        void load(string);

        MaterialInstance& getMaterialInstance() { return m_materialInstance; }
        const MaterialInstance& getMaterialInstance() const { return m_materialInstance; }

    private:

        AssetId m_modelId = InvalidAssetId;
        MaterialInstance m_materialInstance;

    };

}