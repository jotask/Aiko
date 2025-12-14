#pragma once

#include "models/mesh.h"
#include "models/material.h"

namespace aiko
{
    class AssetSystem;
    class AIKO_API Model
    {
        friend class AssetSystem;
        static AssetSystem* s_assetSystem;
    public:

        struct MeshMatData
        {
            Mesh mesh;
            Material material;
        };

        Model();
        ~Model() = default;
        void load(const char*);

    // private:

		std::vector<MeshMatData> m_meshes;

    };
}
