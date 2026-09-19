#include "ui_renderer.h"

#include <algorithm>
#include <cmath>

#include "renderer/Irenderdevice.h"
#include "types/transient_types.h"

namespace aiko
{
    void UIRenderer::beginFrame()
    {
        m_frameMaterials.clear();
    }

    void UIRenderer::setShader(AssetId shaderId)
    {
        m_baseMaterial.m_shaderId = shaderId;
        m_baseMaterial.m_baseColor = WHITE;
        m_baseMaterial.m_useVertexColor = true;
        m_baseMaterial.m_lit = false;
        m_baseMaterial.m_renderState.blend = true;
    }

    void UIRenderer::render(renderer::IRenderDevice& device, const UIDrawList& drawList, const ivec2& surfaceSize)
    {
        if (drawList.empty())
        {
            return;
        }

        TransientGeometry geometry;

        geometry.topology = TransientTopology::Triangles;

        geometry.vertices.reserve(drawList.vertices().size());

        for (const UIVertex& vertex : drawList.vertices())
        {
            TransientVertex transientVertex;

            transientVertex.position =
            {
                vertex.position.x,
                vertex.position.y,
                0.0f
            };

            transientVertex.uv = vertex.uv;
            transientVertex.normal = {0.0f, 0.0f, 1.0f};
            transientVertex.color = vertex.color;
            geometry.vertices.push_back(transientVertex);
        }

        geometry.indices = drawList.indices();

        for (const UIDrawCommand& command : drawList.commands())
        {
            m_frameMaterials.emplace_back();

            Material& material = m_frameMaterials.back();

            material.m_shaderId = m_baseMaterial.m_shaderId;
            material.m_baseColor = WHITE;
            material.m_useVertexColor = true;
            material.m_lit = false;
            material.m_renderState = m_baseMaterial.m_renderState;

            if (command.textureId != InvalidAssetId)
            {
                material.setTexture("u_texture", command.textureId);
            }

            TransientDrawDesc draw;

            draw.mtx = mat4(1.0f);
            draw.material = &material;
            draw.geometry = &geometry;
            draw.indexOffset = command.indexOffset;
            draw.indexCount = command.indexCount;

            if (command.clipRect.has_value())
            {
                const UIRect& clip = *command.clipRect;

                const float left = std::max(0.0f, clip.position.x);
                const float top = std::max(0.0f, clip.position.y);
                const float right = std::min(static_cast<float>(surfaceSize.x), clip.position.x + clip.size.x);
                const float bottom = std::min(static_cast<float>(surfaceSize.y), clip.position.y + clip.size.y);

                if (right <= left || bottom <= top)
                {
                    continue;
                }

                const int32_t scissorLeft = static_cast<int32_t>(std::floor(left));
                const int32_t scissorTop = static_cast<int32_t>(std::floor(top));
                const int32_t scissorRight = static_cast<int32_t>(std::ceil(right));
                const int32_t scissorBottom = static_cast<int32_t>(std::ceil(bottom));

                draw.scissor =
                {
                    .x = scissorLeft,
                    .y = scissorTop,
                    .width = static_cast<uint32_t>(scissorRight - scissorLeft),
                    .height = static_cast<uint32_t>(scissorBottom - scissorTop)
                };
            }

            device.submitTransient(UI_VIEW, draw);
        }
    }

}
