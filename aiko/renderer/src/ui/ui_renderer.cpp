#include "ui_renderer.h"

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
    }

    void UIRenderer::render(renderer::IRenderDevice& device, const UIDrawList& drawList)
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

            device.submitTransient(UI_VIEW, draw);
        }
    }

}
