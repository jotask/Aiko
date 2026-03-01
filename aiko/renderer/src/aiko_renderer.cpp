#include "aiko_renderer.h"

#include <events/events.hpp>

#include "display/display_events.hpp"

#include "render_factory.h"
#include "core/transform.h"
#include "display/display_manager.h"
#include "imgui/aiko_imgui.h"
#include "models/camera.h"

namespace aiko
{
    AikoRenderer::AikoRenderer()
            : m_renderer(renderer::RendererFactory::createRenderDevice())
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
        m_screenFbo.create(size.x, size.y);

        // bind to on window resize
        EventSystem::it().bind<WindowResizeEvent>(this, &AikoRenderer::onWindowResize);

        m_imgui.init(IMGUI_VIEW, DisplayManager::it().getNativeWindow());

    }

    void AikoRenderer::beginFrame()
    {
        m_queue.clear();
        m_instancedQueue.clear();
        m_computeQueue.clear();
        m_lights.clear();
        m_renderer->beginFrame();
        m_imgui.beginFrame();
    }

    void AikoRenderer::endFrame()
    {
        m_renderer->endFrame();
        const auto size = DisplayManager::it().getDisplay()->getDisplaySize();
        m_imgui.endFrame(size.x, size.y);
        m_renderer->present();
    }

    void AikoRenderer::dispose()
    {
        m_renderer->shutdown();
    }

    void AikoRenderer::setBackgroundColor(const Color color)
    {
        m_background_color = color;
    }

    void AikoRenderer::submit(const AmbientLight& ambient, const std::vector<LightData>& data)
    {
        m_ambientLight = ambient;
        m_lights.insert(m_lights.end(), data.begin(), data.end());
    }

    void AikoRenderer::submit(const Transform& transform, const Mesh& mesh, const Material& material)
    {
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

    void AikoRenderer::enqueueCompute(const ComputePass& pass)
    {
        m_computeQueue.push_back(pass);
    }

    void AikoRenderer::render(const Camera& camera)
    {

        const ivec2 size = DisplayManager::it().getDisplay()->getDisplaySize();

        // Pass 0 : To offscreen frame buffer
        {

            const renderer::PassDescription pass =
            {
                .width = static_cast<u32>(size.x),
                .height = static_cast<u32>(size.y),
                .clearColor = true,
                .clearDepth = true,
                .clear = m_background_color
            };

            FrameBuffer fbo = m_screenFbo.getFrameBuffer();
            m_renderer->beginPass(SCENE_VIEW, pass, &fbo);

            const renderer::FrameData frameData =
            {
                .view = camera.getViewMatrix(),
                .projection = camera.getProjectionMatrix(),
                .cameraPosition = camera.position,
                .ambient = m_ambientLight,
                .lights = m_lights,
            };
            m_renderer->bindFrame(SCENE_VIEW, frameData);

            for (const ComputePass& pass : m_computeQueue)
            {
                m_renderer->execute(COMPUTE_VIEW, pass);
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
                std::ranges::sort(m_instancedQueue, [](const InstanceItem& a, const InstanceItem b)
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
            m_renderer->presentFrameBufferToScreen(SCREEN_VIEW, m_screenFbo);
            m_renderer->endPass();

        }

    }

    void AikoRenderer::drawText(string str, float x, float y, float size, Color color)
    {

    }

    FrameBuffer AikoRenderer::getTargetTexture() const
    {
        return m_screenFbo.getFrameBuffer();
    }

    void AikoRenderer::onWindowResize(Event& event)
    {
        const auto& msg = static_cast<const WindowResizeEvent&>(event);
        m_renderer->resize(msg.width, msg.height, false);
        m_screenFbo.resize(msg.width, msg.height);
    }
}
