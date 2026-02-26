#include "aiko_renderer.h"

#include <events/events.hpp>
#include "display/display_events.hpp"

#include "render_factory.h"
#include "core/transform.h"
#include "display/display_manager.h"
#include "imgui/aiko_imgui.h"
#include "models/camera.h"
#include "models/model.h"

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

    void AikoRenderer::submitInstanced(const Mesh& mesh, const Material& material, std::vector<InstanceData> instance)
    {
        InstancedItem item =
        {
            .mesh = &mesh,
            .material = &material
        };
        item.instances.assign(instance.begin(), instance.end());
        m_instancedQueue.push_back(item);
    }

    void AikoRenderer::render(const Camera& camera)
    {

        const ivec2 size = DisplayManager::it().getDisplay()->getDisplaySize();

        // Pass 0 : To offscreen frame buffer
        {

            renderer::PassDescription pass =
            {
                .width = static_cast<u32>(size.x),
                .height = static_cast<u32>(size.y),
                .clearColor = true,
                .clearDepth = true,
                .clear = m_background_color
            };

            FrameBuffer fbo = m_screenFbo.getFrameBuffer();
            m_renderer->beginPass(SCENE_VIEW, pass, &fbo);

            const mat4 view = camera.getViewMatrix();
            const mat4 projection = camera.getProjectionMatrix();
            m_renderer->setViewTransform(SCENE_VIEW, view, projection);

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

            m_renderer->endPass();

        }

        // Pass 1. To backbuffer
        {

            renderer::PassDescription presentPass
            {
                .width = static_cast<u32>(size.x),
                .height = static_cast<u32>(size.y),
                .clearColor = true,
                .clearDepth = true,
                .clear = MAGENTA
            };

            m_renderer->beginPass(SCREEN_VIEW, presentPass, nullptr);
            m_renderer->setViewTransform(SCREEN_VIEW, mat4(1.0f), mat4(1.0f));
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
