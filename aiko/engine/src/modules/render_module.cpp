#include "render_module.h"

#include "modules/module_connector.h"

#include <aiko_renderer.h>

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
