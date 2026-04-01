#include "aiko_renderer.h"

#include <events/events.hpp>
#include <intrumentor/profiler.h>

#include "display/display_events.hpp"

#include "render_factory.h"
#include "core/transform.h"
#include "display/display_manager.h"
#include "imgui/aiko_imgui.h"
#include "models/camera.h"

namespace aiko
{
    AikoRenderer::AikoRenderer(IAssetProvider& assets, IAssetRegistry* registry)
            : m_renderer(renderer::RendererFactory::createRenderDevice(&m_resources))
            , m_assetRegistry(registry)
            , m_resources(assets)
            , m_background_color(RAYWHITE)
    {

    }

    void AikoRenderer::init()
    {

        auto* window = DisplayManager::it().getNativeWindow();
        AIKO_ASSERT(window, "No window created!")
        const ivec2 size = DisplayManager::it().getDisplay()->getDisplaySize();

        renderer::DeviceInitDesc description =
        {
            .nativeWindowHandle = window,
            .width = static_cast<u32>(size.x),
            .height = static_cast<u32>(size.y),
            .vsync = false,
        };

        if (m_renderer->init(description) == false)
        {
            AIKO_ASSERT(false, "Renderer not initialized");
            std::abort();
        }

        // Create Screen fbo
        if (m_screenFbo.isValid() == true)
        {
            m_screenFbo.unload();
        }
        Material screenFboMaterial = {};
        screenFboMaterial.m_shaderId = m_assetRegistry->registerShader("passthrough");
        screenFboMaterial.m_useVertexColor = false;
        screenFboMaterial.m_lit = false;
        screenFboMaterial.m_baseColor = WHITE;
        AIKO_ASSERT(screenFboMaterial.m_shaderId != InvalidAssetId, "ScreenFbo shader invalid!");
        m_screenFbo.setMaterial(std::move(screenFboMaterial));

        m_screenFbo.create(size.x, size.y);

        // bind to on window resize
        EventSystem::it().bind<WindowResizeEvent>(this, &AikoRenderer::onWindowResize);

        m_imgui.init(IMGUI_VIEW, DisplayManager::it().getNativeWindow());

    }

    void AikoRenderer::beginFrame()
    {
        AIKO_FUNCTION_PROFILE;
        m_queue.clear();
        m_transientQueue.clear();
        m_instancedQueue.clear();
        m_gpuBillboardQueue.clear();
        m_computeQueue.clear();
        m_gpuInstanceDraws.clear();
        m_lights.clear();
        m_renderer->beginFrame();
        m_imgui.beginFrame();
    }

    void AikoRenderer::endFrame()
    {
        AIKO_FUNCTION_PROFILE;
        m_renderer->endFrame();
        const auto size = DisplayManager::it().getDisplay()->getDisplaySize();
        m_imgui.endFrame(size.x, size.y);
        m_renderer->present();
        AIKO_FRAME_MARK
    }

    void AikoRenderer::dispose()
    {
        m_renderer->shutdown();
    }

    void AikoRenderer::setBackgroundColor(const Color color)
    {
        m_background_color = color;
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
        RenderItem item =
        {
            .mesh = &mesh,
            .material = &material,
            .transform = transform.getMatrix()
        };
        m_queue.push_back(item);
    }

    void AikoRenderer::submit(const Mesh& mesh, const Material& material, const void* data, uint32_t instanceCount, uint16_t stride)
    {
        AIKO_FUNCTION_PROFILE
        if (data == nullptr || instanceCount == 0 || stride == 0)
        {
            return;
        }

        InstanceItem item;
        item.mesh = &mesh;
        item.material = &material;
        item.count = instanceCount;
        item.stride = stride;

        const size_t byteCount = size_t(instanceCount) * size_t(stride);
        item.data.resize(byteCount);
        memcpy(item.data.data(), data, byteCount);

        m_instancedQueue.push_back(std::move(item));
    }

    void AikoRenderer::submitTransient(const Transform& transform, const Material& material, const MeshAsset& meshAsset, TransientTopology topology)
    {
        AIKO_FUNCTION_PROFILE
        TransientDrawDesc data = {};
        data.mtx = transform.getMatrix();
        data.material = &material;
        data.topology = topology;

        data.vertices.reserve(meshAsset.m_vertices.size());

        for (uint32_t i = 0; i < static_cast<uint32_t>(meshAsset.m_vertices.size()); ++i)
        {

            TransientVertex v = {};

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

            data.vertices.push_back(v);
        }

        if (meshAsset.m_indices.empty() == false)
        {
            data.indices = meshAsset.m_indices;
        }

        m_transientQueue.push_back(std::move(data));

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
        m_gpuInstanceDraws.push_back(desc);
    }

    void AikoRenderer::drawBillboards(const GpuBillboardDrawDesc& desc)
    {
        m_gpuBillboardQueue.push_back(desc);
    }

    void AikoRenderer::render(const Camera& camera)
    {
        AIKO_FUNCTION_PROFILE
        const ivec2 size = DisplayManager::it().getDisplay()->getDisplaySize();

        const renderer::FrameData frameData =
        {
            .view = camera.getViewMatrix(),
            .projection = camera.getProjectionMatrix(),
            .cameraPosition = camera.position,
            .ambient = m_ambientLight,
            .lights = m_lights,
        };

        // PASS 0 — COMPUTE
        {
            renderer::PassDescription computePass{};
            computePass.width  = 1;
            computePass.height = 1;

            m_renderer->beginPass(COMPUTE_VIEW, computePass, nullptr);

            for (const ComputePass& pass : m_computeQueue)
                m_renderer->execute(COMPUTE_VIEW, pass);

            m_renderer->endPass();
        }

        // Pass 0 : To offscreen frame buffer
        {
            renderer::PassDescription pass =
            {
                .width  = (u32)size.x,
                .height = (u32)size.y,
                .clearColor = true,
                .clearDepth = true,
                .clear = m_background_color
            };

            const FrameBuffer& fbo = m_screenFbo.getFrameBuffer();

            m_renderer->beginPass(SCENE_VIEW, pass, &fbo);
            m_renderer->bindFrame(SCENE_VIEW, frameData);

            for (auto& desc : m_gpuInstanceDraws)
            {
                m_renderer->drawMeshInstancedGpu(SCENE_VIEW, desc);
            }

            for (const GpuBillboardDrawDesc& desc : m_gpuBillboardQueue)
            {
                m_renderer->drawBillboards(SCENE_VIEW, desc);
            }

            std::ranges::sort(m_queue, [](const RenderItem& a, const RenderItem& b)
            {
                const auto aId = a.material != nullptr ? a.material->id() : 0;
                const auto bId = b.material != nullptr ? b.material->id() : 0;
                return aId < bId;
            });

            u64 lastMaterialId = ~0ull;
            const Material* lastMaterial = nullptr;

            constexpr const bool useBatching = true;

            for (const RenderItem& item : m_queue)
            {
                if (item.mesh == nullptr || item.material == nullptr)
                {
                    continue;
                }
                const auto materialId = item.material->id();
                if (materialId != lastMaterialId)
                {
                    lastMaterialId = materialId;
                    lastMaterial = item.material;
                    if constexpr (useBatching == true)
                    {
                        m_renderer->bindMaterial(*lastMaterial);
                    }
                }
                if constexpr (useBatching == true)
                {
                    m_renderer->drawMesh(SCENE_VIEW, item.transform, *item.mesh, *item.material);
                }
                else
                {
                    m_renderer->renderMesh(SCENE_VIEW, item.transform, *item.mesh, *item.material );
                }
            }

            // Instanced batch
            {
                std::ranges::sort(m_instancedQueue, [](const InstanceItem& a, const InstanceItem& b)
                {
                    const u64 aMat = a.material ? a.material->id() : 0;
                    const u64 bMat = b.material ? b.material->id() : 0;
                    if (aMat != bMat) return aMat < bMat;
                    const u32 aMesh = a.mesh ? a.mesh->id() : 0;
                    const u32 bMesh = b.mesh ? b.mesh->id() : 0;
                    return aMesh < bMesh;
                });

                u64 lastMaterialId = ~0ull;
                for (auto& batch : m_instancedQueue)
                {
                    if (batch.mesh == nullptr || batch.material == nullptr || batch.data.empty())
                    {
                        continue;
                    }
                    u64 materialId = batch.material->id();
                    if (materialId != lastMaterialId)
                    {
                        lastMaterialId = materialId;
                        m_renderer->bindMaterial(*batch.material);
                    }
                    m_renderer->drawMeshInstanced(SCENE_VIEW, *batch.mesh, *batch.material, batch.data.data(), batch.count, batch.stride);
                }
            }

            {

                std::ranges::sort(m_transientQueue, [](const TransientDrawDesc& a, const TransientDrawDesc& b)
                {
                    const auto aId = a.material != nullptr ? a.material->id() : 0;
                    const auto bId = b.material != nullptr ? b.material->id() : 0;
                    return aId < bId;
                });

                u64 lastMaterialId = ~0ull;
                for (const TransientDrawDesc& data : m_transientQueue)
                {
                    if (data.material == nullptr || data.vertices.empty())
                    {
                        continue;
                    }
                    u64 materialId = data.material->id();
                    if (materialId != lastMaterialId)
                    {
                        lastMaterialId = materialId;
                        m_renderer->bindMaterial(*data.material);
                    }
                    m_renderer->drawTransient(SCENE_VIEW, data);
                }
            }

            m_renderer->endPass();

        }

        // Pass 1. To backbuffer
        {

            const renderer::PassDescription presentPass
            {
                .width = static_cast<u32>(size.x),
                .height = static_cast<u32>(size.y),
                .clearColor = true,
                .clearDepth = true,
                .clear = MAGENTA
            };

            const renderer::FrameData screenFrame =
            {
                .view = mat4(1.0f),
                .projection = mat4(1.0f),
            };

            m_renderer->beginPass(SCREEN_VIEW, presentPass, nullptr);
            m_renderer->bindFrame(SCREEN_VIEW, screenFrame);

            if (m_debugTexture != nullptr && m_debugTexture->isValid())
            {
                m_renderer->presentTextureToScreen(SCREEN_VIEW, m_screenFbo, *m_debugTexture);
            }
            else
            {
                m_renderer->presentFrameBufferToScreen(SCREEN_VIEW, m_screenFbo);
            }

            m_renderer->endPass();

        }

    }

    void AikoRenderer::setDebugTexture(const Texture* texture)
    {
        m_debugTexture = texture;
    }

    const FrameBuffer& AikoRenderer::getTargetTexture() const
    {
        return m_screenFbo.getFrameBuffer();
    }

    void AikoRenderer::onWindowResize(WindowResizeEvent& event)
    {
        m_renderer->resize(event.width, event.height, false);
        m_screenFbo.resize(event.width, event.height);
    }
}
