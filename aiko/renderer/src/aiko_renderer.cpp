#include "aiko_renderer.h"

#include <events/events.hpp>
#include <intrumentor/profiler.h>

#include "display/display_events.hpp"

#include "render_factory.h"
#include "core/transform.h"
#include "time/time.h"
#include "imgui/aiko_imgui.h"
#include "models/camera.h"

#include <unordered_set>

namespace aiko
{
    AikoRenderer::AikoRenderer(IAssetProvider& assets)
        : m_resources(assets)
        , m_renderer(renderer::RendererFactory::createRenderDevice(&m_resources))
    {

    }

    void AikoRenderer::init(const RendererConfig& config, const RenderSurfaceDesc& surface)
    {

        AIKO_ASSERT(surface.nativeWindowHandle != nullptr, "No window created!");

        m_renderSurface =
        {
            static_cast<int>(surface.width),
            static_cast<int>(surface.height)
        };

        const DeviceInitDesc description =
        {
            .nativeWindowHandle = surface.nativeWindowHandle,
            .width = surface.width,
            .height = surface.height,
            .vsync = config.vsync,
        };

        if (m_renderer->init(description) == false)
        {
            AIKO_ASSERT(false, "Renderer not initialized");
            std::abort();
        }

        // Create scene render target
        if (m_sceneTarget.isValid() == true)
        {
            m_sceneTarget.unload();
        }

        m_sceneTarget.create(surface.width, surface.height);

        m_screenPresenter.init();
        AIKO_ASSERT(m_screenPresenter.isValid(), "Screen presenter invalid");

        // bind to on window resize
        EventSystem::it().bind<WindowResizeEvent>(this, &AikoRenderer::onWindowResize);

        m_imgui.init(static_cast<GLFWwindow*>(surface.nativeWindowHandle));

    }

    void AikoRenderer::beginFrame()
    {
        AIKO_FUNCTION_PROFILE;

        if (m_pendingSurfaceResize.has_value())
        {
            m_renderSurface = *m_pendingSurfaceResize;
            m_renderer->resize(m_renderSurface.x, m_renderSurface.y);
            m_sceneTarget.resize(m_renderSurface.x, m_renderSurface.y);
            m_pendingSurfaceResize.reset();
        }

        m_renderer->beginFrame();

        m_frameMaterials.clear();
        m_frameMaterialCache.clear();
        m_renderQueue.clear();
        m_computeQueue.clear();
        m_lights.clear();
        m_imgui.beginFrame(m_renderSurface.x, m_renderSurface.y);
    }

    void AikoRenderer::endFrame()
    {
        AIKO_FUNCTION_PROFILE;
        m_renderer->endFrame();
        m_renderer->present();
        AIKO_FRAME_MARK
    }

    void AikoRenderer::dispose()
    {
        m_imgui.dispose();
        m_transientGeometryCache.clear();
        m_screenPresenter.dispose();
        m_sceneTarget.unload();
        m_resources.clear();
        m_renderer->shutdown();
    }

    void AikoRenderer::setClearColor(Color color)
    {
        m_clearColor = color;
    }

    void AikoRenderer::submit(const AmbientLight& ambient, const vector<LightData>& data)
    {
        AIKO_FUNCTION_PROFILE
        m_ambientLight = ambient;
        m_lights.insert(m_lights.end(), data.begin(), data.end());
    }

    void AikoRenderer::submit(const Transform& transform, const Mesh& mesh, const Material& material)
    {
        AIKO_FUNCTION_PROFILE
        m_renderQueue.submit(mesh, material, transform.getWorldMatrix());
    }

    void AikoRenderer::submit(const Mesh& mesh, const Material& material, const void* data, uint32_t instanceCount, uint16_t stride)
    {
        AIKO_FUNCTION_PROFILE
        m_renderQueue.submitInstanced(mesh, material, data, instanceCount, stride);
    }

    void AikoRenderer::submit(const Transform& transform, const Mesh& mesh, const MaterialAsset& materialAsset, const MaterialInstance& materialInstance)
    {
        AIKO_FUNCTION_PROFILE
        Material& stagedMaterial = stageMaterial(materialAsset, materialInstance);
        submit(transform, mesh, stagedMaterial);
    }

    void AikoRenderer::submitTransient(const Transform& transform, const Material& material, const MeshAsset& meshAsset, TransientTopology topology)
    {
        AIKO_FUNCTION_PROFILE
        TransientDrawDesc draw = {};
        draw.mtx = transform.getWorldMatrix();
        draw.material = &material;
        draw.topology = topology;
        draw.geometry = &resolveTransientGeometry(meshAsset, topology);
        m_renderQueue.submitTransient(std::move(draw));
    }

    void AikoRenderer::enqueueCompute(const ComputePass& pass)
    {
        m_computeQueue.push_back(pass);
    }

    void AikoRenderer::requestReadback(const ComputeReadbackRequest& req)
    {
        m_renderer->requestReadback(req);
    }

    bool AikoRenderer::pollReadback(ComputeReadbackResult& out)
    {
        return m_renderer->pollReadback(out);
    }

    void AikoRenderer::drawMeshInstancedGpu(const GpuInstanceDrawDesc& desc)
    {
        m_renderQueue.submitGpuInstances(desc);
    }

    void AikoRenderer::drawBillboards(const GpuBillboardDrawDesc& desc)
    {
        m_renderQueue.submitGpuBillboards(desc);
    }

    void AikoRenderer::drawVerticesGpu(const GpuVertexDrawDesc& desc)
    {
        m_renderQueue.submitGpuVertices(desc);
    }

    void AikoRenderer::render(const Camera& camera)
    {
        AIKO_FUNCTION_PROFILE
        const renderer::FrameData frameData = buildSceneFrameData(camera);

        m_renderer->bindFrame(COMPUTE_VIEW, frameData);
        executeComputePasses();

        const PreparedScenePass scenePass = m_renderQueue.buildScenePass();
        submitScenePass(frameData, scenePass, m_renderSurface);

        submitPresentPass(m_renderSurface);

    }

    void AikoRenderer::setDebugTexture(const Texture* texture)
    {
        m_debugTexture = texture;
    }

    void AikoRenderer::waitIdle()
    {
        m_renderer->waitIdle();
    }

    void AikoRenderer::onWindowResize(WindowResizeEvent& event)
    {
        if (event.width <= 0 || event.height <= 0)
        {
            return;
        }
        m_pendingSurfaceResize =
        {
            event.width,
            event.height
        };
    }

    renderer::FrameData AikoRenderer::buildSceneFrameData(const Camera& camera) const
    {
        AIKO_FUNCTION_PROFILE
        const renderer::FrameData frameData =
        {
            .view = camera.getViewMatrix(),
            .projection = camera.getProjectionMatrix(m_renderSurface),
            .cameraPosition = camera.position,
            .time = static_cast<float>( Time::it().secondSinceStart()),
            .deltaTime = Time::it().getDeltaTime(),
            .ambient = m_ambientLight,
            .lights = m_lights,
        };
        return frameData;
    }

    void AikoRenderer::executeComputePasses()
    {
        AIKO_FUNCTION_PROFILE
        const renderer::PassDescription computePass
        {
            .width = 1,
            .height = 1
        };

        m_renderer->beginPass(COMPUTE_VIEW, computePass, nullptr);

        for (const ComputePass& pass : m_computeQueue)
        {
            m_renderer->execute(COMPUTE_VIEW, pass);
        }

        m_renderer->endPass();
    }

    void AikoRenderer::submitScenePass(const renderer::FrameData& frameData, const PreparedScenePass& passData, const ivec2& size)
    {
        AIKO_FUNCTION_PROFILE
        const renderer::PassDescription pass =
        {
            .width = static_cast<u32>(size.x),
            .height = static_cast<u32>(size.y),
            .clearColor = true,
            .clearDepth = true,
            .clear = m_clearColor
        };

        const FrameBuffer& target = m_sceneTarget.frameBuffer();

        std::unordered_set<const Material*> preparedMaterials;

        auto prepareMaterial = [&](const Material* material)
            {
                if (material == nullptr)
                {
                    return;
                }

                if (preparedMaterials.insert(material).second)
                {
                    m_renderer->prepareMaterial(*material);
                }
            };

        for (const GpuInstanceDrawDesc* desc : passData.gpuInstances)
        {
            if (desc == nullptr)
            {
                continue;
            }
            prepareMaterial(desc->material);
            m_renderer->prepareGpuReadBuffers(desc->readBuffers);
        }

        for (const GpuBillboardDrawDesc* desc : passData.gpuBillboards)
        {
            if (desc == nullptr)
            {
                continue;
            }

            prepareMaterial(desc->material);

            m_renderer->prepareGpuReadBuffers(
                {
                    {
                        .slot = 7,
                        .buffer = desc->positionBuffer,
                    }
                });
        }

        for (const GpuVertexDrawDesc* desc : passData.gpuVertices)
        {
            if (desc == nullptr)
            {
                continue;
            }

            prepareMaterial(desc->material);

            if (desc->vertexBuffer != nullptr)
            {
                m_renderer->prepareVertexBuffer(*desc->vertexBuffer);
            }

            if (desc->indexBuffer != nullptr)
            {
                m_renderer->prepareIndexBuffer(*desc->indexBuffer);
            }

            if (desc->indirectBuffer != nullptr)
            {
                m_renderer->prepareIndirectBuffer(*desc->indirectBuffer);
            }
        }

        for (const PreparedRenderPacket& packet : passData.opaque)
        {
            prepareMaterial(packet.draw.material);
        }

        for (const PreparedInstancedPacket& packet : passData.instanced)
        {
            prepareMaterial(packet.draw.material);
        }

        for (const PreparedTransientPacket& packet : passData.transient)
        {
            if (packet.item != nullptr)
            {
                prepareMaterial(packet.item->material);
            }
        }

        m_renderer->beginPass(SCENE_VIEW, pass, &target);
        m_renderer->bindFrame(SCENE_VIEW, frameData);

        {
            for (const GpuVertexDrawDesc* desc : passData.gpuVertices)
            {
                if (desc != nullptr)
                {
                    m_renderer->submitGpuVertices(SCENE_VIEW, *desc);
                }
            }
        }

        {
            for (const GpuInstanceDrawDesc* desc : passData.gpuInstances)
            {
                if (desc != nullptr)
                {
                    m_renderer->submitGpuInstanced(SCENE_VIEW, *desc);
                }
            }
        }

        {
            for (const GpuBillboardDrawDesc* desc : passData.gpuBillboards)
            {
                if (desc != nullptr)
                {
                    m_renderer->submitBillboards(SCENE_VIEW, *desc);
                }
            }
        }

        {
            for (const PreparedRenderPacket& packet : passData.opaque)
            {
                AIKO_ASSERT(packet.draw.mesh != nullptr, "Prepared opaque packet has null mesh");
                AIKO_ASSERT(packet.draw.material != nullptr, "Prepared opaque packet has null material");

                m_renderer->submitMesh(SCENE_VIEW, packet.draw);
            }
        }

        {
            for (const PreparedInstancedPacket& packet : passData.instanced)
            {
                AIKO_ASSERT(packet.draw.mesh != nullptr, "Prepared instanced packet has null mesh");
                AIKO_ASSERT(packet.draw.material != nullptr, "Prepared instanced packet has null material");
                AIKO_ASSERT(packet.draw.data != nullptr, "Prepared instanced packet has null data");

                m_renderer->submitInstanced(SCENE_VIEW, packet.draw);
            }
        }

        {
            for (const PreparedTransientPacket& packet : passData.transient)
            {
                AIKO_ASSERT(packet.item != nullptr, "Prepared transient packet has null item");
                m_renderer->submitTransient(SCENE_VIEW, *packet.item);
            }
        }

        m_renderer->endPass();
    }

    void AikoRenderer::submitPresentPass(const ivec2& size)
    {
        AIKO_FUNCTION_PROFILE
        const renderer::PassDescription presentPass
        {
            .width = static_cast<u32>(size.x),
            .height = static_cast<u32>(size.y),
            .clearColor = false,
            .clearDepth = false,
        };

        const renderer::FrameData screenFrame =
        {
            .view = mat4(1.0f),
            .projection = mat4(1.0f),
        };

        const Texture* presentTexture = &m_sceneTarget.colorTexture();

        if (m_debugTexture != nullptr && m_debugTexture->isValid())
        {
            presentTexture = m_debugTexture;
        }

        AIKO_ASSERT(presentTexture->isValid(), "Present texture is invalid");

        m_renderer->prepareTextureForSampling(*presentTexture);

        m_renderer->beginPass(SCREEN_VIEW, presentPass, nullptr);
        m_renderer->bindFrame(SCREEN_VIEW, screenFrame);

        m_renderer->presentTextureToScreen(SCREEN_VIEW, m_screenPresenter.mesh(), *presentTexture);

        m_imgui.endFrame(size.x, size.y);

        m_renderer->endPass();
    }

    Material& AikoRenderer::stageMaterial(const MaterialAsset& materialAsset, const MaterialInstance& materialInstance)
    {
        AIKO_FUNCTION_PROFILE
        FrameMaterialKey key{};
        key.shaderId =
            materialInstance.shaderId != InvalidAssetId
                ? materialInstance.shaderId
                : materialAsset.shaderId;

        key.diffuseTextureId = materialAsset.diffuseTextureId;
        key.runtimeDiffuseTexture = materialInstance.runtimeDiffuseTexture;
        key.useVertexColor = materialAsset.useVertexColor;
        key.lit = materialAsset.lit;
        key.baseColor = materialAsset.baseColor;

        if (auto it = m_frameMaterialCache.find(key); it != m_frameMaterialCache.end())
        {
            AIKO_ASSERT(it->second != nullptr, "Cached frame material pointer is null");
            return *it->second;
        }

        m_frameMaterials.emplace_back();
        Material& material = m_frameMaterials.back();

        material.m_shaderId = key.shaderId;
        material.m_gpuInstanceBuffer = nullptr;

        material.m_useVertexColor = key.useVertexColor;
        material.m_lit = key.lit;
        material.m_baseColor = key.baseColor;

        if (key.runtimeDiffuseTexture != nullptr)
        {
            material.setTexture("u_texture", key.runtimeDiffuseTexture);
        }
        else if (key.diffuseTextureId != InvalidAssetId)
        {
            material.setTexture("u_texture", key.diffuseTextureId);
        }

        // Optional prewarm. Not a major perf win, but ensures shader exists before draw.
        if (material.m_shaderId != InvalidAssetId)
        {
            (void)m_resources.getShader(material.m_shaderId);
        }

        m_frameMaterialCache.emplace(key, &material);
        return material;
    }

    const TransientGeometry& AikoRenderer::resolveTransientGeometry(const MeshAsset& meshAsset,
        TransientTopology topology)
    {
        AIKO_FUNCTION_PROFILE

   const TransientCacheKey key
   {
       .meshAsset = &meshAsset,
       .topology = topology
   };

        if (auto it = m_transientGeometryCache.find(key); it != m_transientGeometryCache.end())
        {
            return it->second;
        }

        TransientGeometry geometry{};
        geometry.topology = topology;
        geometry.vertices.reserve(meshAsset.m_vertices.size());

        for (uint32_t i = 0; i < static_cast<uint32_t>(meshAsset.m_vertices.size()); ++i)
        {
            TransientVertex v{};

            v.position = meshAsset.m_vertices[i];

            if (i < meshAsset.m_textCoord.size())
            {
                v.uv = meshAsset.m_textCoord[i];
            }

            if (i < meshAsset.m_normals.size())
            {
                v.normal = meshAsset.m_normals[i];
            }

            if (i < meshAsset.m_colors.size())
            {
                v.color = meshAsset.m_colors[i];
            }
            else
            {
                v.color = WHITE;
            }

            geometry.vertices.push_back(v);
        }

        if (!meshAsset.m_indices.empty())
        {
            geometry.indices = meshAsset.m_indices;
        }

        auto [it, inserted] = m_transientGeometryCache.emplace(key, std::move(geometry));
        AIKO_ASSERT(inserted, "Failed to cache transient geometry");
        return it->second;
    }
}
