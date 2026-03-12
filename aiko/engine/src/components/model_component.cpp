#include "model_component.h"

#include "constants.h"
#include "models/game_object.h"
#include "assets/asset_manager.h"

namespace aiko
{

    ModelComponent::ModelComponent()
        : Component("Model")
    {

    }

    void ModelComponent::load(string path)
    {
        IComponentAssetAccess* assets = gameobject->getAiko()->getComponentAssetAccess();
        setModelId(assets->registerModel(path));
    }
}
