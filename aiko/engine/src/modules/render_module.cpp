#include "render_module.h"

#include "modules/module_connector.h"

#include <aiko_renderer.h>

#include "display/display_manager.h"
#include "models/camera.h"

namespace aiko
{
    RenderModule::RenderModule(Aiko* aiko)
        : BaseModule(aiko)
        , m_mainCamera(nullptr)
    {
    }

    void RenderModule::setMainCamera(const Camera* camera)
    {
        m_mainCamera = camera;
    }

    void RenderModule::submitLights(const AmbientLight& ambient, const std::vector<LightData>& data)
    {
        AikoRenderer::it().submit(ambient, data);
    }

    void RenderModule::init()
    {
        AikoRenderer::it().init();

        auto size = DisplayManager::it().getDisplay()->getDisplaySize();

        const auto width = size.x;
        const auto height = size.y;

        // output texture (NOT a framebuffer attachment)
        if (texture.isValid())
            texture.unload();

        texture.create({
            .type = texture::TextureType::Sampled,
            .format = texture::TextureFormat::RGBA8,
            .width = width,
            .height = height,
            .mipmaps = false,
            .computeWrite = true
        });

        shader.load("particles.cs");

        const uint32_t count = uint32_t(width) * uint32_t(height);
        std::vector<vec4> initData(count, vec4(0, 0, 0, 1));
        buffer.createVec4(count, initData.data());

    }

    void RenderModule::update()
    {
        BaseModule::update();
    }

    void RenderModule::render()
    {
        static float m_time = 0.0f;

        auto size = DisplayManager::it().getDisplay()->getDisplaySize();

        const auto m_width = size.x;
        const auto m_height = size.y;

        m_time += 0.016f; // replace later with real dt

        ComputePass pass;
        pass.shader = &shader;

        pass.buffers.push_back({ 0, &buffer, ComputeAccess::ReadWrite });
        pass.images.push_back({ 1, &texture, ComputeAccess::Write });

        pass.dispatch.groupsX = (uint32_t(m_width) + 7) / 8;
        pass.dispatch.groupsY = (uint32_t(m_height) + 7) / 8;
        pass.dispatch.groupsZ = 1;

        // uniform
        pass.vec4Uniforms.push_back({ "u_params", vec4(m_time, float(m_width), float(m_height), 0.0f) });

        AikoRenderer::it().enqueueCompute(pass);

    }

    void RenderModule::beginFrame()
    {
        m_instances.clear();
        AikoRenderer::it().beginFrame();
    }

    void RenderModule::endFrame()
    {
        AIKO_ASSERT(m_mainCamera != nullptr, "Main camera not set. Forgot to call set Main camera?");
        AikoRenderer::it().render(*m_mainCamera);
        AikoRenderer::it().endFrame();
        m_instances.clear();
    }

    void RenderModule::dispose()
    {
        AikoRenderer::it().dispose();
    }

}
