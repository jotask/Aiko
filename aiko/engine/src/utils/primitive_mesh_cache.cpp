#include "primitive_mesh_cache.h"

#include "models/mesh_factory.h"

namespace aiko
{

    void PrimitiveMeshCache::init()
    {
        m_triangleMesh = std::make_unique<MeshAsset>( mesh::factory::generateTriangle());
        m_rectangleMesh = std::make_unique<MeshAsset>( mesh::factory::generateQuad());
        m_pyramidMesh = std::make_unique<MeshAsset>( mesh::factory::generatePyramid());
        m_cubeMesh = std::make_unique<MeshAsset>( mesh::factory::generateCube());
        m_torusMesh = std::make_unique<MeshAsset>( mesh::factory::generateMeshTorus());
        m_knotMesh = std::make_unique<MeshAsset>( mesh::factory::generateMeshKnot());
    }

    MeshAsset& PrimitiveMeshCache::getOrCreateCircleMesh(uint segments)
    {
        AIKO_ASSERT(segments >= 3, "Mesh cirlcle should contain at least 3 segments");
        auto it = m_circleCache.find(segments);
        if (it != m_circleCache.end())
        {
            return *it->second;
        }
        AikoUPtr<MeshAsset> mesh = std::make_unique<MeshAsset>(mesh::factory::generateCircle(segments));
        MeshAsset& ref = *mesh;
        m_circleCache.emplace(segments, std::move(mesh));
        return ref;
    }

    MeshAsset& PrimitiveMeshCache::getOrCreateGridMesh(ivec2 resolution)
    {
        AIKO_ASSERT(resolution.x >= 2, "Mesh grid resolution to slow");
        AIKO_ASSERT(resolution.y >= 2, "Mesh grid resolution to slow");
        const uint32_t x = static_cast<uint32_t>(resolution.x);
        const uint32_t y = static_cast<uint32_t>(resolution.y);
        const uint64_t key = makeKey(x, y);

        auto it = m_gridCache.find(key);
        if (it != m_gridCache.end())
        {
            return *it->second;
        }

        AikoUPtr<MeshAsset> mesh = std::make_unique<MeshAsset>(mesh::factory::generateMeshPlane(1.0f, 1.0f, resolution.x, resolution.y));
        MeshAsset& ref = *mesh;
        m_gridCache.emplace(key, std::move(mesh));
        return ref;
    }

    MeshAsset& PrimitiveMeshCache::getOrCreateSphereMesh(int rings, int sectors)
    {

        AIKO_ASSERT(rings >= 3, "Mesh sphere too few rings");
        AIKO_ASSERT(sectors >= 3, "Mesh sphere to few sectors");

        const uint32_t r = static_cast<uint32_t>(rings);
        const uint32_t s = static_cast<uint32_t>(sectors);
        const uint64_t key = makeKey(r, s);

        auto it = m_sphereCache.find(key);
        if (it != m_sphereCache.end())
        {
            return *it->second;
        }
        AikoUPtr<MeshAsset> mesh = std::make_unique<MeshAsset>(mesh::factory::generateMeshSphere(rings, sectors));
        MeshAsset& ref = *mesh;
        m_sphereCache.emplace(key, std::move(mesh));
        return ref;
    }

    MeshAsset& PrimitiveMeshCache::getOrCreateCylinderMesh(uint sectors)
    {
        AIKO_ASSERT(sectors >= 3, "Mesh cylinder should contain more sectors");
        auto it = m_cylinderCache.find(sectors);
        if (it != m_cylinderCache.end())
        {
            return *it->second;
        }
        AikoUPtr<MeshAsset> mesh = std::make_unique<MeshAsset>(mesh::factory::generateMeshCylinder(sectors));
        MeshAsset& ref = *mesh;
        m_cylinderCache.emplace(sectors, std::move(mesh));
        return ref;
    }

    uint64_t PrimitiveMeshCache::makeKey(uint32_t a, uint32_t b) const
    {
        return (uint64_t(a) << 32) | uint64_t(b);
    }

}
