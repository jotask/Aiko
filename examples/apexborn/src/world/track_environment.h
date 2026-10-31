#pragma once

#include "models/material.h"
#include "models/mesh.h"

#include <math/math_vector.h>
#include <cstdint>

namespace aiko
{
    class AssetContext;
    class RenderContext;
}

namespace ab
{

    class TrackEnvironment final
    {
    public:
        void init(aiko::AssetContext& assets);

        bool generate(const aiko::vec2& size, uint32_t resolution = 65);

        void render(aiko::RenderContext& context) const;

    private:
        aiko::Mesh m_groundMesh;
        aiko::Material m_groundMaterial;
    };

}
