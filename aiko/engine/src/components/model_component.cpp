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

    void ModelComponent::resolveAssetBinding(AssetBindingContext& context)
    {
        if (m_model.isRequested() == false)
        {
            return;
        }

        m_model.markLoading();

        const AssetId id = context.load<ModelAsset>(m_model.source());

        if (id == InvalidAssetId)
        {
            m_model.fail();
            return;
        }

        m_model.resolve(id);
    }

}
