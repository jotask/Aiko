#include "model_component.h"

#include "systems/asset_system.h"
#include "models/game_object.h"

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

}
