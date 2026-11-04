#include "model_component.h"

#include <utility>

namespace aiko
{
    ModelComponent::ModelComponent()
        : Component("Model")
    {
    }

    void ModelComponent::load(string path)
    {
        m_source = std::move(path);

        // Detach this component from its previous model.
        // Do not unload shared resource state.
        m_model = Model{};

        m_modelId = InvalidAssetId;
        m_loadRequested = !m_source.empty();
    }

    void ModelComponent::resolveAssetBinding(AssetBindingContext& context)
    {
        if (!m_loadRequested)
        {
            return;
        }

        const AssetId id = context.load<ModelAsset>(m_source);

        m_loadRequested = false;

        if (id == InvalidAssetId)
        {
            return;
        }

        m_model = context.loadModel(id);
        m_modelId = id;
    }
}
