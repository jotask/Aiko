#pragma once

#include "aiko_types.h"
#include "models/component.h"
#include "metadata/material_instance.h"
#include "assets/asset_reference.h"
#include "assets/types/mesh_asset.h"
#include "assets/asset_binding.h"

namespace aiko
{

    class ModelComponent : public Component, public IAssetBinding
    {
    public:

        ModelComponent();
        virtual ~ModelComponent() = default;

        void load(string path);

        const AssetId& getModelId() const;

        MaterialInstance& getMaterialInstance() { return m_materialInstance; }
        const MaterialInstance& getMaterialInstance() const { return m_materialInstance; }

    private:

        void resolveAssetBinding(AssetBindingContext& context) override;

        AssetReference<ModelAsset> m_model;
        MaterialInstance m_materialInstance;

    };

}