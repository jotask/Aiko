#pragma once

#include "assets/types/mesh_asset.h"

#include <math/math_vector.h>
#include <types/color.h>

#include <aiko_types.h>

namespace sph
{

    struct ShapeEdge
    {
        size_t a = 0;
        size_t b = 0;
    };

    class Shape
    {
    public:
        Shape(aiko::vec3 position, aiko::MeshAsset asset, aiko::Color color);

        bool isPointInside(const aiko::vec3& worldPoint) const;
        bool getDirectionOut(const aiko::vec3& worldPoint, float radius, aiko::vec3& out) const;
        void moveBy(const aiko::vec3& offset);

        void setPosition(const aiko::vec3& position) { m_position = position; }
        void setColor(const aiko::Color& color) { m_color = color; }
        void setScale(const aiko::vec3& scale) { m_scale = scale; }

        bool getNearestVector(const aiko::vec3& worldPoint, const float affectDistance, aiko::vec3& out) const;

        const aiko::vector<ShapeEdge>& boundaryEdges() const { return m_boundaryEdges; }
        const aiko::vec3& position() const { return m_position; }
        const aiko::vec3& scale() const { return m_scale; }
        const aiko::MeshAsset& asset() const { return m_asset; }
        const aiko::Color& color() const { return m_color; }

    private:
        void calculateBoundaryEdges();

        aiko::vec3 scaledVertex(size_t index) const;

        static aiko::vec3 closestPointOnSegment(const aiko::vec3& point, const aiko::vec3& a, const aiko::vec3& b);

        aiko::vec3 m_position = {};

        aiko::vec3 m_scale = { 1.0f, 1.0f, 1.0f };

        aiko::MeshAsset m_asset;
        aiko::Color m_color = aiko::ORANGE;

        aiko::vector<ShapeEdge> m_boundaryEdges;
    };

}