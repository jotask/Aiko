#pragma once

#include <models/mesh.h>

#include <aiko_types.h>

#include <unordered_map>

namespace aiko
{

    class PrimitiveMeshCache
    {
    public:

        void init();
        void dispose();

        Mesh& getPointMesh() const { return *m_pointMesh.get(); }
        Mesh& getLineMesh() const { return *m_lineMesh.get(); }
        Mesh& getTriangleMesh() const { return *m_triangleMesh.get(); }
        Mesh& getRectangleMesh() const { return *m_rectangleMesh.get(); }
        Mesh& getPyramidMesh() const { return *m_pyramidMesh.get(); }
        Mesh& getCubeMesh() const { return *m_cubeMesh.get(); }
        Mesh& getTorusMesh() const { return *m_torusMesh.get(); }
        Mesh& getKnotMesh() const { return *m_knotMesh.get(); }

        Mesh& getOrCreateCircleMesh(uint segments);
        Mesh& getOrCreateGridMesh(ivec2 resolution);
        Mesh& getOrCreateSphereMesh(int rings, int sectors);
        Mesh& getOrCreateCylinderMesh(uint sectors);

    private:

        using MeshCache = std::unordered_map<uint64_t, AikoUPtr<Mesh>>;

        AikoUPtr<Mesh> m_pointMesh = nullptr;
        AikoUPtr<Mesh> m_lineMesh = nullptr;
        AikoUPtr<Mesh> m_triangleMesh = nullptr;
        AikoUPtr<Mesh> m_rectangleMesh = nullptr;
        AikoUPtr<Mesh> m_pyramidMesh = nullptr;
        AikoUPtr<Mesh> m_cubeMesh = nullptr;

        // TODO : Currently we store default parameters, as it's just test types
        AikoUPtr<Mesh> m_torusMesh = nullptr;
        AikoUPtr<Mesh> m_knotMesh = nullptr;
        // ENDTODO

        MeshCache m_circleCache;
        MeshCache m_gridCache;
        MeshCache m_sphereCache;
        MeshCache m_cylinderCache;

        uint64_t makeKey(uint32_t a, uint32_t b) const;
    };

}