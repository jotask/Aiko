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

    bool ModelComponent::resolveAssetBinding(AssetBindingContext& context)
    {
        if (m_model.isRequested() == false)
        {
            return false;
        }

        m_model.markLoading();

        const AssetId id = context.load<ModelAsset>(m_model.source());

        if (id == InvalidAssetId)
        {
            m_model.fail();
            return false;
        }

        m_model.resolve(id);
        return false;
    }

}
