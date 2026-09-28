#include <shapes/shape.h>

namespace sph
{

    Shape::Shape(aiko::vec3 position, aiko::MeshAsset asset, aiko::Color color)
        : m_position(position)
        , m_asset(asset)
        , m_color(color)
    {

    }

    bool Shape::isPointInside(const aiko::vec3& worldPoint) const
    {
        const aiko::vec3 point = worldPoint - m_position;
        bool inside = false;
        for (size_t i = 0, j = m_asset.m_vertices.size() - 1; i < m_asset.m_vertices.size(); j = i++)
        {
            const aiko::vec3& a = m_asset.m_vertices[i];
            const aiko::vec3& b = m_asset.m_vertices[j];
            const bool intersects = ((a.y > point.y) != (b.y > point.y)) && (point.x < (b.x - a.x) * (point.y - a.y) / (b.y - a.y) + a.x);
            if (intersects)
            {
                inside = !inside;
            }
        }

        return inside;
    }

    void Shape::moveBy(const aiko::vec3& offset)
    {
        m_position += offset;
    }

}
