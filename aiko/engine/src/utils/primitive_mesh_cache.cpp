#include "primitive_mesh_cache.h"

#include "models/mesh_factory.h"

namespace aiko
{
    void PrimitiveMeshCache::init()
    {
        #define GEN_MESH(var, mesh)             \
        {                                       \
            (var) = std::make_unique<Mesh>();   \
            (var)->upload((mesh));              \
        }

        GEN_MESH(m_pointMesh, mesh::factory::generatePoint());
        GEN_MESH(m_triangleMesh, mesh::factory::generateTriangle());
        GEN_MESH(m_rectangleMesh, mesh::factory::generateQuad());
        GEN_MESH(m_pyramidMesh, mesh::factory::generatePyramid());
        GEN_MESH(m_cubeMesh, mesh::factory::generateCube());
        GEN_MESH(m_torusMesh, mesh::factory::generateMeshTorus());
        GEN_MESH(m_knotMesh, mesh::factory::generateMeshKnot());

    }

    void PrimitiveMeshCache::dispose()
    {
        #define DISPOSE_CACHE(var)                              \
        {                                                       \
            for (auto& tmp : (var)) tmp.second->unload();       \
            (var).clear();                                      \
        }

        DISPOSE_CACHE(m_lineCache);
        DISPOSE_CACHE(m_circleCache);
        DISPOSE_CACHE(m_gridCache);
        DISPOSE_CACHE(m_sphereCache);
        DISPOSE_CACHE(m_cylinderCache);

        m_pointMesh->unload();
        m_triangleMesh->unload();
        m_rectangleMesh->unload();
        m_pyramidMesh->unload();
        m_cubeMesh->unload();
        m_torusMesh->unload();
        m_knotMesh->unload();

    }

    Mesh& PrimitiveMeshCache::getOrCreateLineMesh(vec3 start, vec3 end)
    {

        auto floatBits = [](float v) -> uint32_t
        {
            uint32_t bits;
            std::memcpy(&bits, &v, sizeof(uint32_t));
            return bits;
        };

        auto hashVec3 = [floatBits](vec3 v) -> uint64_t
        {
            uint64_t h = 1469598103934665603ull;

            auto mix = [&h](uint32_t bits)
            {
                h ^= uint64_t(bits);
                h *= 1099511628211ull;
            };

            mix(floatBits(v.x));
            mix(floatBits(v.y));
            mix(floatBits(v.z));
            return h;
        };

        auto makeLineKey = [hashVec3](vec3 start, vec3 end) -> uint64_t
        {
            uint64_t a = hashVec3(start);
            uint64_t b = hashVec3(end);
            return a ^ (b + 0x9e3779b97f4a7c15ull + (a << 6) + (a >> 2));
        };

        const uint64_t key = makeLineKey(start, end);

        auto it = m_lineCache.find(key);
        if (it != m_lineCache.end())
        {
            return *it->second;
        }

        auto mesh = std::make_unique<Mesh>();
        mesh->upload(mesh::factory::generateLine(start, end));

        Mesh& ref = *mesh;
        m_lineCache.emplace(key, std::move(mesh));
        return ref;
    }

    Mesh& PrimitiveMeshCache::getOrCreateCircleMesh(uint segments)
    {
        AIKO_ASSERT(segments >= 3, "Mesh cirlcle should contain at least 3 segments");
        auto it = m_circleCache.find(segments);
        if (it != m_circleCache.end())
        {
            return *it->second;
        }

        auto mesh = std::make_unique<Mesh>();
        mesh->upload(mesh::factory::generateCircle(segments));

        Mesh& ref = *mesh;
        m_circleCache.emplace(segments, std::move(mesh));
        return ref;
    }

    Mesh& PrimitiveMeshCache::getOrCreateGridMesh(ivec2 resolution)
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

        auto mesh = std::make_unique<Mesh>();
        mesh->upload(mesh::factory::generateMeshPlane(1.0f, 1.0f, resolution.x, resolution.y));

        Mesh& ref = *mesh;
        m_gridCache.emplace(key, std::move(mesh));
        return ref;
    }

    Mesh& PrimitiveMeshCache::getOrCreateSphereMesh(int rings, int sectors)
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

        auto mesh = std::make_unique<Mesh>();
        mesh->upload(mesh::factory::generateMeshSphere(rings, sectors));

        Mesh& ref = *mesh;
        m_sphereCache.emplace(key, std::move(mesh));
        return ref;
    }

    Mesh& PrimitiveMeshCache::getOrCreateCylinderMesh(uint sectors)
    {
        AIKO_ASSERT(sectors >= 3, "Mesh cylinder should contain more sectors");
        auto it = m_cylinderCache.find(sectors);
        if (it != m_cylinderCache.end())
        {
            return *it->second;
        }

        auto mesh = std::make_unique<Mesh>();
        mesh->upload(mesh::factory::generateMeshCylinder(sectors));

        Mesh& ref = *mesh;
        m_cylinderCache.emplace(sectors, std::move(mesh));
        return ref;
    }

    uint64_t PrimitiveMeshCache::makeKey(uint32_t a, uint32_t b) const
    {
        return (uint64_t(a) << 32) | uint64_t(b);
    }
}
