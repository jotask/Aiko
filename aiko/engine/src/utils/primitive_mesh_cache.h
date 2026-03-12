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

        MeshAsset& getTriangleMesh() const { return *m_triangleMesh.get(); }
        MeshAsset& getRectangleMesh() const { return *m_rectangleMesh.get(); }
        MeshAsset& getPyramidMesh() const { return *m_pyramidMesh.get(); }
        MeshAsset& getCubeMesh() const { return *m_cubeMesh.get(); }
        MeshAsset& getTorusMesh() const { return *m_torusMesh.get(); }
        MeshAsset& getKnotMesh() const { return *m_knotMesh.get(); }

        MeshAsset& getOrCreateCircleMesh(uint segments);
        MeshAsset& getOrCreateGridMesh(ivec2 resolution);
        MeshAsset& getOrCreateSphereMesh(int rings, int sectors);
        MeshAsset& getOrCreateCylinderMesh(uint sectors);

    private:

        using MeshCache = std::unordered_map<uint64_t, AikoUPtr<MeshAsset>>;

        AikoUPtr<MeshAsset> m_triangleMesh = nullptr;
        AikoUPtr<MeshAsset> m_rectangleMesh = nullptr;
        AikoUPtr<MeshAsset> m_pyramidMesh = nullptr;
        AikoUPtr<MeshAsset> m_cubeMesh = nullptr;

        // TODO : Currently we store default parameters, as it's just test types
        AikoUPtr<MeshAsset> m_torusMesh = nullptr;
        AikoUPtr<MeshAsset> m_knotMesh = nullptr;
        // ENDTODO

        MeshCache m_circleCache;
        MeshCache m_gridCache;
        MeshCache m_sphereCache;
        MeshCache m_cylinderCache;

        uint64_t makeKey(uint32_t a, uint32_t b) const;

    };

}