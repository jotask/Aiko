#pragma once

#include "models/mesh.h"
#include "models/material.h"

namespace aiko
{
    class Model
    {
        friend class AssetSystem;
        static AssetSystem* s_assetSystem;
    public:
        Model();
        ~Model() = default;
        void load(const char*);
    public:
        Mesh m_mesh;
        Material m_material;
    };
}
