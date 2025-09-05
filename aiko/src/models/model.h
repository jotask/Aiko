#pragma once

#include "models/mesh.h"
#include "models/material.h"

namespace aiko
{
    class Model
    {
    public:
        Model();
        ~Model() = default;
    public:
        Mesh m_mesh;
        Material m_material;
    };
}
