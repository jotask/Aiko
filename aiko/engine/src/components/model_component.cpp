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
        markAssetBindingDirty();
    }

    const AssetId& ModelComponent::getModelId() const
    {
        return m_model.isReady() ? m_model.id() : InvalidAssetId;
    }

    bool ModelComponent::resolveAssetBinding(AssetBindingContext& context)
    {
        if (m_model.isRequested())
        {
            const AssetId id = context.load<ModelAsset>(m_model.source());

            if (id == InvalidAssetId)
            {
                m_model.fail();
                return false;
            }

            m_model.markLoading(id);
            context.loadAsset<ModelAsset>(id);

            return true;
        }

        if (m_model.isLoading())
        {
            const AssetId& id = m_model.id();

            if (context.isLoaded<ModelAsset>(id) == false)
            {
                return true;
            }

            m_model.resolve(id);
        }

        return false;
    }

}
