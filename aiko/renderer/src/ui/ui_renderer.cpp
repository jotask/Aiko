#include "ui_renderer.h"

#include <algorithm>
#include <cmath>
#include <vector>

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

        // The geometry is still uploaded only once per pass by the existing
        // transient upload cache. Each UI quad uses normal.x for its FLAT
        // style slot in the current batch; the other normal components are
        // not consumed by the UI shader.
        TransientGeometry geometry;
        geometry.topology = TransientTopology::Triangles;
        geometry.vertices.reserve(drawList.vertices().size());

        for (const UIVertex& vertex : drawList.vertices())
        {
            TransientVertex transientVertex;
            transientVertex.position = {vertex.position.x, vertex.position.y, 0.0f};
            transientVertex.uv = vertex.uv;
            transientVertex.normal = {0.0f, 0.0f, 1.0f};
            transientVertex.color = vertex.color;
            geometry.vertices.push_back(transientVertex);
        }

        geometry.indices = drawList.indices();

        static constexpr uint32_t MaxUiBatch = 128;

        struct Batch
        {
            AssetId textureId = InvalidAssetId;
            SamplerState sampler{};
            std::optional<ScissorRect> scissor;
            uint32_t firstIndex = 0;
            uint32_t indexCount = 0;
            uint32_t styleCount = 0;

            vector<vec3> positionsRadius = vector<vec3>(MaxUiBatch);
            vector<vec3> sizesBorder = vector<vec3>(MaxUiBatch);
            vector<vec3> borderRgb = vector<vec3>(MaxUiBatch);
            vector<float> borderAlpha = vector<float>(MaxUiBatch);
        };

        auto sameScissor = [](const std::optional<ScissorRect>& a, const std::optional<ScissorRect>& b)
        {
            if (a.has_value() != b.has_value())
            {
                return false;
            }
            if (!a.has_value())
            {
                return true;
            }
            return a->x == b->x && a->y == b->y &&
                   a->width == b->width && a->height == b->height;
        };

        std::vector<TransientDrawDesc> pendingDraws;
        pendingDraws.reserve(drawList.commands().size());

        Batch batch;

        auto flush = [&]()
        {
            if (batch.styleCount == 0)
            {
                return;
            }

            // deque keeps submitted Material addresses stable until beginFrame().
            m_frameMaterials.emplace_back();
            Material& material = m_frameMaterials.back();
            material.m_shaderId = m_baseMaterial.m_shaderId;
            material.m_baseColor = WHITE;
            material.m_useVertexColor = true;
            material.m_lit = false;
            material.m_renderState = m_baseMaterial.m_renderState;

            if (batch.textureId != InvalidAssetId)
            {
                material.setTexture("u_texture", batch.textureId, batch.sampler);
            }

            // These arrays are exactly the reflected shader array length.
            material.setVec3Array("u_uiRectPosRadius", std::move(batch.positionsRadius));
            material.setVec3Array("u_uiRectSizeBorder", std::move(batch.sizesBorder));
            material.setVec3Array("u_uiBorderRgb", std::move(batch.borderRgb));
            material.setFloatArray("u_uiBorderAlpha", std::move(batch.borderAlpha));

            TransientDrawDesc draw;
            draw.mtx = mat4(1.0f);
            draw.material = &material;
            draw.geometry = &geometry;
            draw.indexOffset = batch.firstIndex;
            draw.indexCount = batch.indexCount;
            draw.scissor = batch.scissor;

            pendingDraws.push_back(draw);

            batch = Batch{};
        };

        for (const UIDrawCommand& command : drawList.commands())
        {
            std::optional<ScissorRect> scissor;
            if (command.clipRect.has_value())
            {
                const UIRect& clip = *command.clipRect;
                const float left = std::max(0.0f, clip.position.x);
                const float top = std::max(0.0f, clip.position.y);
                const float right = std::min(static_cast<float>(surfaceSize.x), clip.position.x + clip.size.x);
                const float bottom = std::min(static_cast<float>(surfaceSize.y), clip.position.y + clip.size.y);

                if (right <= left || bottom <= top)
                {
                    // A skipped command creates an index gap. Never merge
                    // across it or later geometry would be drawn incorrectly.
                    flush();
                    continue;
                }

                const int32_t scissorLeft = static_cast<int32_t>(std::floor(left));
                const int32_t scissorTop = static_cast<int32_t>(std::floor(top));
                const int32_t scissorRight = static_cast<int32_t>(std::ceil(right));
                const int32_t scissorBottom = static_cast<int32_t>(std::ceil(bottom));
                scissor = ScissorRect{
                    .x = scissorLeft,
                    .y = scissorTop,
                    .width = static_cast<uint32_t>(scissorRight - scissorLeft),
                    .height = static_cast<uint32_t>(scissorBottom - scissorTop),
                };
            }

            AIKO_ASSERT(command.indexOffset <= geometry.indices.size(), "UI command index offset out of range");
            AIKO_ASSERT(command.indexCount <= geometry.indices.size() - command.indexOffset, "UI command index range out of bounds");

            if (command.indexCount == 0)
            {
                flush();
                continue;
            }

            const bool canAppend =
                batch.styleCount != 0 &&
                batch.styleCount < MaxUiBatch &&
                batch.firstIndex + batch.indexCount == command.indexOffset &&
                batch.textureId == command.textureId &&
                batch.sampler == command.sampler &&
                sameScissor(batch.scissor, scissor);

            if (batch.styleCount != 0 && !canAppend)
            {
                flush();
            }

            if (batch.styleCount == 0)
            {
                batch.textureId = command.textureId;
                batch.sampler = command.sampler;
                batch.scissor = scissor;
                batch.firstIndex = command.indexOffset;
            }

            const uint32_t slot = batch.styleCount++;
            batch.indexCount += command.indexCount;
            batch.positionsRadius[slot] = vec3(command.rect.position.x, command.rect.position.y, command.cornerRadius);
            batch.sizesBorder[slot] = vec3(command.rect.size.x, command.rect.size.y, command.borderThickness);
            batch.borderRgb[slot] = vec3(command.borderColor.r, command.borderColor.g, command.borderColor.b);
            batch.borderAlpha[slot] = command.borderColor.a;

            // Every vertex referenced by this primitive receives the same
            // style index; the shader forwards it as a FLAT integer.
            for (uint32_t i = 0; i < command.indexCount; ++i)
            {
                const uint32_t vertexIndex = geometry.indices[command.indexOffset + i];
                AIKO_ASSERT(vertexIndex < geometry.vertices.size(), "UI vertex index out of bounds");
                geometry.vertices[vertexIndex].normal.x = static_cast<float>(slot);
            }
        }

        flush();

        // All style indices are assigned before the first GPU upload.
        for (const TransientDrawDesc& draw : pendingDraws)
        {
            device.submitTransient(UI_VIEW, draw);
        }
    }

}
