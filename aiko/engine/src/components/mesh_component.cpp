#include "mesh_component.h"

#include "models/game_object.h"
#include "models/mesh_factory.h"
#include "systems/asset_system.h"

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
        AssetSystem* assets = context().assets;
        AIKO_ASSERT(assets != nullptr, "Asset system not found");
        setMeshId(assets->load<MeshAsset>(path));
    }

    void MeshComponent::loadDebugCube()
    {
        AssetSystem* assets = context().assets;
        AIKO_ASSERT(assets != nullptr, "Asset system not found");
        const AssetId cubeMeshId = assets->create(aiko::mesh::factory::generateCube());
        setMeshId(cubeMeshId);
        m_material.shaderId = assets->load<ShaderAsset>("model");
    }

    void MeshComponent::loadMesh(const MeshAsset& asset)
    {
        AssetSystem* assets = context().assets;
        AIKO_ASSERT(assets != nullptr, "Asset system not found");
        const AssetId meshId = assets->create(asset);
        setMeshId(meshId);
        m_material.shaderId = assets->load<ShaderAsset>("model");
    }
}
