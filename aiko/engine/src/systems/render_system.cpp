#include "render_system.h"

#include <limits>

#include <aiko_types.h>

#include "modules/module_connector.h"
#include "systems/asset_system.h"
#include "modules/render_module.h"
#include "systems/system_connector.h"
#include "systems/scene_system.h"
#include "components/mesh_component.h"
#include "components/sprite_component.h"
#include "components/model_component.h"
#include "assets/types/mesh_asset.h"
#include "types/builtin_shaders.h"
#include "models/font.h"
#include "ui/text_layout.h"
#include <intrumentor/profiler.h>

namespace aiko
{

    RenderSystem::RenderSystem()
    {
    }

    void RenderSystem::init()
    {
        m_materialPrimitives.m_shaderId = m_assetSystem->registerAsset<ShaderAsset>(renderer::BuiltinShader::Model);
        m_assetSystem->loadAsset<ShaderAsset>(m_materialPrimitives.m_shaderId);

        const AssetId uiShader = m_assetSystem->registerAsset<ShaderAsset>(renderer::BuiltinShader::UI);

        m_assetSystem->loadAsset<ShaderAsset>(uiShader);

        m_renderModule->setUiShader(uiShader);

        m_primitiveMeshCache.init();
    }

    void RenderSystem::dispose()
    {
        BaseSystem::dispose();
        clearCaches();
    }

    void RenderSystem::submitMesh(const Transform& trans, const AssetId& meshId, const Material& material)
    {
        AIKO_FUNCTION_PROFILE
        AIKO_ASSERT(meshId != InvalidAssetId, "MeshComponent has no mesh id assigned");
        Mesh& mesh = m_renderModule->getMesh(meshId);
        render(trans, mesh, material);
    }

    void RenderSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        BIND_MODULE_REQUIRED(RenderModule, moduleConnector, m_renderModule)
        BIND_SYSTEM_REQUIRED(AssetSystem, systemConnector, m_assetSystem)
        BIND_SYSTEM_REQUIRED(SceneSystem, systemConnector, m_sceneSystem)
    }

    void RenderSystem::clearCaches()
    {
        m_worldTextMaterials.clear();
    }

    ImguiTextureId RenderSystem::getTargetTextureId() const
    {
        return m_renderModule->getTargetTextureId();
    }

    void RenderSystem::drawUiRect(const vec2& position, const vec2& size, Color color, float cornerRadius, float borderThickness, Color borderColor)
    {
        m_renderModule->drawUiRect(position, size, color, cornerRadius, borderThickness, borderColor);
    }

    void RenderSystem::drawUiImage(AssetId textureId, const vec2& position, const vec2& size, Color tint, float cornerRadius, float borderThickness, Color borderColor)
    {
        m_renderModule->drawUiImage(textureId, position, size, tint, cornerRadius, borderThickness, borderColor);
    }

    void RenderSystem::drawUiImage(AssetId textureId, const TextureRegion& region, const vec2& position, const vec2& size, Color tint, float cornerRadius, float borderThickness, Color borderColor)
    {
        m_renderModule->drawUiImage(textureId, region, position, size, tint, cornerRadius, borderThickness, borderColor);
    }

    void RenderSystem::drawUiText(const Font& font, string_view text, const vec2& position, float fontSize, Color color)
    {
        if (font.isValid() == false)
        {
            return;
        }
        const TextLayoutResult layout = layoutText(font, text, fontSize);
        for (const TextGlyphQuad& glyph : layout.glyphs)
        {
            TextureRegion region;
            region.min = glyph.uvMin;
            region.max = glyph.uvMax;
            drawUiImage(font.atlasTexture(), region, position + glyph.position, glyph.size, color);
        }
    }

    void RenderSystem::drawText(const Font& font, string_view text, const Transform& transform, float fontSize, Color color)
    {
        if (font.isValid() == false || text.empty() || fontSize <= 0.0f)
        {
            return;
        }

        const TextLayoutResult layout = layoutText(font, text, fontSize);

        if (layout.glyphs.empty())
        {
            return;
        }

        MeshAsset mesh;
        mesh.m_vertices.reserve(layout.glyphs.size() * 4);
        mesh.m_textCoord.reserve(layout.glyphs.size() * 4);
        mesh.m_normals.reserve(layout.glyphs.size() * 4);
        mesh.m_colors.reserve(layout.glyphs.size() * 4);
        mesh.m_indices.reserve(layout.glyphs.size() * 6);

        for (const TextGlyphQuad& glyph : layout.glyphs)
        {
            AIKO_ASSERT(mesh.m_vertices.size() <= std::numeric_limits<uint16_t>::max() - 4, "World text exceeded the 16-bit vertex limit");

            const uint16_t base = static_cast<uint16_t>(mesh.m_vertices.size());

            const float left = glyph.position.x;
            const float right = glyph.position.x + glyph.size.x;
            const float top = -glyph.position.y;
            const float bottom = -(glyph.position.y + glyph.size.y);

            mesh.m_vertices.push_back({left, top, 0.0f});
            mesh.m_vertices.push_back({right, top, 0.0f});
            mesh.m_vertices.push_back({left, bottom, 0.0f});
            mesh.m_vertices.push_back({right, bottom, 0.0f});
            mesh.m_textCoord.push_back({glyph.uvMin.x, glyph.uvMin.y});
            mesh.m_textCoord.push_back({glyph.uvMax.x, glyph.uvMin.y});
            mesh.m_textCoord.push_back({glyph.uvMin.x, glyph.uvMax.y});
            mesh.m_textCoord.push_back({glyph.uvMax.x, glyph.uvMax.y});

            for (int i = 0; i < 4; ++i)
            {
                mesh.m_normals.push_back({0.0f, 0.0f, 1.0f});
                mesh.m_colors.push_back(color);
            }

            mesh.m_indices.push_back(base + 0);
            mesh.m_indices.push_back(base + 2);
            mesh.m_indices.push_back(base + 1);

            mesh.m_indices.push_back(base + 1);
            mesh.m_indices.push_back(base + 2);
            mesh.m_indices.push_back(base + 3);
        }

        auto [materialIt, inserted] = m_worldTextMaterials.try_emplace(font.atlasTexture());

        Material& material = materialIt->second;

        if (inserted)
        {
            material.m_shaderId = m_materialPrimitives.m_shaderId;
            material.m_baseColor = WHITE;
            material.m_useVertexColor = true;
            material.m_lit = false;
            material.m_renderState.cullMode = CullMode::None;
            material.m_renderState.depthTest = true;
            material.m_renderState.depthWrite = false;
            material.m_renderState.blend = true;
            material.setTexture("u_texture", font.atlasTexture());
        }

        m_renderModule->submitTransient(transform, material, mesh, TransientTopology::Triangles);
    }

    void RenderSystem::pushUiClipRect(const vec2& position, const vec2& size)
    {
        m_renderModule->pushUiClipRect(position, size);
    }

    void RenderSystem::popUiClipRect()
    {
        m_renderModule->popUiClipRect();
    }

    void RenderSystem::render(const Transform& trans, const Mesh& mesh, const Material& mat)
    {
        AIKO_FUNCTION_PROFILE
        m_renderModule->submit(trans, mesh, mat);
    }

    void RenderSystem::render(const Transform& trans, const Model& model)
    {
        AIKO_FUNCTION_PROFILE
        for (const auto& submesh : model.getSubMeshes())
        {
            AIKO_ASSERT(submesh.meshId != InvalidAssetId, "Runtime model submesh has invalid mesh id");
            Mesh& mesh = m_renderModule->getMesh(submesh.meshId);
            m_renderModule->submit(trans, mesh, submesh.material);
        }
    }

    void RenderSystem::render(const Transform& trans, const MeshComponent& meshComponent)
    {
        AIKO_FUNCTION_PROFILE
        const AssetId& meshId = meshComponent.getMeshId();
        if (meshId == InvalidAssetId)
        {
            return;
        }
        const Material& material = meshComponent.getMaterial();
        if (material.m_shaderId == InvalidAssetId)
        {
            return;
        }
        if (!m_assetSystem->isLoaded<ShaderAsset>(material.m_shaderId))
        {
            return;
        }
        Mesh& mesh = m_renderModule->getMesh(meshId);
        m_renderModule->submit(trans, mesh, meshComponent.getMaterial());
    }

    void RenderSystem::render(const Transform& trans, const ModelComponent& modelComponent)
    {
        AIKO_FUNCTION_PROFILE
        const AssetId& modelId = modelComponent.getModelId();
        if (modelId == InvalidAssetId)
        {
            return;
        }
        Model& runtimeModel = m_renderModule->getModel(modelId);
        render(trans, runtimeModel);
    }

    void RenderSystem::render(const Transform& trans, const SpriteComponent& spriteComponent)
    {
        AIKO_FUNCTION_PROFILE

        const AssetId& textureId = spriteComponent.getTextureId();
        if (textureId == InvalidAssetId)
        {
            return;
        }

        const Material& material = spriteComponent.getMaterial();
        if (material.m_shaderId == InvalidAssetId)
        {
            return;
        }

        if (m_assetSystem->isLoaded<ShaderAsset>(material.m_shaderId) == false)
        {
            return;
        }

        const TextureRegion& region = spriteComponent.getTextureRegion();

        const vec2& size = spriteComponent.getSize();

        const vec2& pivot = spriteComponent.getPivot();

        const float left = -pivot.x * size.x;
        const float right = left + size.x;

        const float bottom = -pivot.y * size.y;
        const float top = bottom + size.y;

        const bool flipX = spriteComponent.getFlipX();
        const bool flipY = spriteComponent.getFlipY();

        const float minU = flipX ? region.max.x : region.min.x;
        const float maxU = flipX ? region.min.x : region.max.x;

        const float minV = flipY ? region.max.y : region.min.y;
        const float maxV = flipY ? region.min.y : region.max.y;

        MeshAsset quad;

        quad.m_vertices =
        {
            { right, top,    0.0f},
            { right, bottom, 0.0f},
            { left,  bottom, 0.0f},
            { left,  top,    0.0f},
        };

        quad.m_textCoord =
        {
            {maxU, maxV},
            {maxU, minV},
            {minU, minV},
            {minU, maxV},
        };

        quad.m_normals =
        {
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
        };

        quad.m_colors =
        {
            WHITE,
            WHITE,
            WHITE,
            WHITE,
        };

        quad.m_indices =
        {
            0, 1, 3,
            1, 2, 3
        };

        m_renderModule->submitTransient(trans, material, quad, TransientTopology::Triangles);
    }

    void RenderSystem::drawVerticesGpu(const GpuVertexDrawDesc& desc)
    {
        m_renderModule->drawVerticesGpu(desc);
    }

    void RenderSystem::renderInstanced(const Mesh& mesh, const Material& material, const InstanceData* instances, u32 instanceCount)
    {
        AIKO_ASSERT(instances != nullptr, "Instanced render has no instance data");
        AIKO_ASSERT(instanceCount > 0, "Instanced render has zero instances");
        m_renderModule->submitInstanced(mesh, material, instances, instanceCount);
    }

    void RenderSystem::dispatch(const ComputePass& pass, const AssetId& shaderId)
    {
        AIKO_FUNCTION_PROFILE
        AIKO_ASSERT(shaderId != InvalidAssetId, "Attempting to dispatch compute with invalid shader id");
        ComputeShader& shader = m_renderModule->getComputeShader(shaderId);
        ComputePass runtimePass = pass;
        runtimePass.shader = &shader;
        m_renderModule->enqueueCompute(runtimePass);
    }

    void RenderSystem::dispatch(const ComputePass& pass, const ComputeShaderComponent& component)
    {
        AIKO_FUNCTION_PROFILE
        dispatch(pass, component.getShaderId());
    }

    void RenderSystem::requestReadback(const ComputeReadbackRequest& req)
    {
        AIKO_FUNCTION_PROFILE
        m_renderModule->requestReadback(req);
    }

    bool RenderSystem::pollReadback(ComputeReadbackResult& req)
    {
        AIKO_FUNCTION_PROFILE
        return m_renderModule->pollReadback(req);
    }

    void RenderSystem::renderToTarget(const Camera& camera, RenderTarget& target)
    {
        m_renderModule->renderToTarget(camera, target);
    }

    const FrameBuffer& RenderSystem::getTargetTexture() const
    {
        return m_renderModule->getTargetTexture();
    }

    Camera* RenderSystem::getMainCamera()
    {
        return m_sceneSystem->getMainCamera();
    }
}
