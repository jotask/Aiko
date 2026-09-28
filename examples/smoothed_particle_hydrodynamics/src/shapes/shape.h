#pragma once

#include "assets/types/mesh_asset.h"

#include <math/math_vector.h>
#include <types/color.h>

#include <aiko_types.h>

namespace sph
{

    class Shape
    {
    public:
        Shape(aiko::vec3 position, aiko::MeshAsset asset, aiko::Color color);
        bool isPointInside(const aiko::vec3& worldPoint) const;

        void moveBy(const aiko::vec3& offset);

        const aiko::vec3& position() const { return m_position; }
        const aiko::MeshAsset& asset() const { return m_asset; }
        const aiko::Color& color() const { return m_color; }

    private:
        aiko::vec3 m_position = {};
        aiko::MeshAsset m_asset;
        aiko::Color m_color = aiko::ORANGE;
    };

}
