#include "track_environment.h"

#include "assets/types/mesh_asset.h"
#include "core/transform.h"
#include "layers/contexts/asset_context.h"
#include "layers/contexts/render_context.h"
#include "models/mesh_factory.h"

#include <cmath>

namespace ab
{
    namespace
    {
        constexpr float GroundHeight = -0.12f;
        constexpr aiko::Color GroundColor = { 0.23f, 0.35f, 0.22f};
    }

    void TrackEnvironment::init(aiko::AssetContext& assets)
    {
        m_groundMaterial.m_shaderId = assets.loadShader("model");
        m_groundMaterial.m_lit = true;
        m_groundMaterial.m_useVertexColor = false;
        m_groundMaterial.m_baseColor = GroundColor;
    }

    bool TrackEnvironment::generate(const aiko::vec2& size, uint32_t resolution)
    {
        if (!aiko::math::isfinite(size.x) || !aiko::math::isfinite(size.y) || size.x <= 0.0f || size.y <= 0.0f || resolution < 2 || resolution > 256)
        {
            return false;
        }

        auto asset = aiko::mesh::factory::generateMeshPlane(size.x, size.y, static_cast<int>(resolution), static_cast<int>(resolution));

        for (auto& vertex : asset.m_vertices)
        {
            // TODO Generate 3D noise for more visual appealing terrain
            vertex.y = GroundHeight;
        }

        aiko::mesh::factory::recalculateNormals(asset);

        m_groundMesh.upload(asset);
        return true;
    }

    void TrackEnvironment::render(aiko::RenderContext& context) const
    {
        if (!m_groundMesh.isValid())
        {
            return;
        }

        aiko::Transform transform{};
        context.drawMesh(transform, m_groundMesh, m_groundMaterial);
    }
}
