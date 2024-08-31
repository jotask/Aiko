#pragma once

#include "models/mesh.h"

namespace aiko
{
    class Model
    {
    public:
        Model() = default;
        ~Model() = default;
    private:
        Mesh m_mesh;
    };
}
