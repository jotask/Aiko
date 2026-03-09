#include "mesh_component.h"

#include "assets/asset_manager.h"
#include "models/game_object.h"
#include "models/mesh_factory.h"

namespace aiko
{

    MeshComponent::MeshComponent()
        : Component("Mesh")
    {
    }

    void MeshComponent::init()
    {

    }

    void MeshComponent::load(string path)
    {
        IComponentAssetAccess* assets = gameobject->getAiko()->getComponentAssetAccess();
        setMeshId(assets->registerMesh(path));
    }

    void MeshComponent::loadDebugCube()
    {
        IComponentAssetAccess* assets = gameobject->getAiko()->getComponentAssetAccess();
        const AssetId cubeMeshId = assets->registerMesh(aiko::mesh::factory::generateCube());
        setMeshId(cubeMeshId);
    }
}
