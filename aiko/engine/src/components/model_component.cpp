#include "model_component.h"

namespace aiko
{

    ModelComponent::ModelComponent()
        : Component("Model")
    {

    }

    void ModelComponent::load(string path)
    {
        m_model.request(std::move(path));
    }

    const AssetId& ModelComponent::getModelId() const
    {
        return m_model.isReady() ? m_model.id() : InvalidAssetId;
    }

    void ModelComponent::resolveAssetBinding(AssetBindingContext& context)
    {
        if (m_model.isRequested())
        {
            const AssetId id = context.load<ModelAsset>(m_model.source());

            if (id == InvalidAssetId)
            {
                m_model.fail();
                return;
            }

            m_model.markLoading(id);
            context.loadAsset<ModelAsset>(id);

            return;
        }

        if (m_model.isLoading())
        {
            const AssetId& id = m_model.id();

            if (context.isLoaded<ModelAsset>(id) == false)
            {
                return;
            }

            m_model.resolve(id);
        }

    }

}
