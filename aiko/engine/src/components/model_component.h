#pragma once

#include "aiko_types.h"
#include "models/component.h"
#include "models/model.h"
#include "assets/asset_id.h"
#include "assets/asset_binding.h"

namespace aiko
{
    class ModelComponent : public Component, public IAssetBinding
    {
    public:
        ModelComponent();
        ~ModelComponent() override = default;

        void load(string path);

        [[nodiscard]]
        const Model& getModel() const
        {
            return m_model;
        }

        [[nodiscard]]
        const AssetId& getModelId() const
        {
            return m_modelId;
        }

        [[nodiscard]]
        const string& getAssetSource() const
        {
            return m_source;
        }

    private:
        void resolveAssetBinding(AssetBindingContext& context) override;

        string m_source;
        Model m_model;
        AssetId m_modelId = InvalidAssetId;
        bool m_loadRequested = false;
    };
}
