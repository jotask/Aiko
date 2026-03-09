#include "compute_shader_component.h"

#include "models/mesh_factory.h"
#include "assets/icomponent_assetaccess.h"
#include "models/game_object.h"

namespace aiko
{
    ComputeShaderComponent::ComputeShaderComponent()
        : Component("ComputeShaderComponent")
    {
    }

    void ComputeShaderComponent::load(string path)
    {
        AIKO_ASSERT(path.empty() == false, "Attempting to load empty compute shader path");
        IComponentAssetAccess* accessor = gameobject->getAiko()->getComponentAssetAccess();
        AIKO_ASSERT(accessor != nullptr, "No AssetSystem available");
        setShaderId(accessor->registerComputeShader(path));
    }

}
