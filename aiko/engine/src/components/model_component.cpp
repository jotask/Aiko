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
        AssetSystem* assets = context().assets;
        AIKO_ASSERT(assets != nullptr, "Asset system not found");
        setModelId(assets->load<ModelAsset>(path));
    }
}
