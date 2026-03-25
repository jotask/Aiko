#include "render_module.h"

#include "modules/module_connector.h"

#include <aiko_renderer.h>

#include "assets_manager_module.h"
#include "display/display_manager.h"
#include "models/camera.h"
#include "models/mesh_factory.h"
#include "time/time.h"

namespace aiko
{
    RenderModule::RenderModule(Aiko* aiko)
        : BaseModule(aiko)
        , m_mainCamera(nullptr)
        , m_renderer(nullptr)
    {
    }

    void RenderModule::setMainCamera(const Camera* camera)
    {
        m_mainCamera = camera;
    }

    void RenderModule::submitLights(const AmbientLight& ambient, const std::vector<LightData>& data)
    {
        m_renderer->submit(ambient, data);
    }

    void RenderModule::connect(ModuleConnector* moduleConnector)
    {
        BIND_MODULE_REQUIRED(AssetsManagerModule, moduleConnector, m_assetManager);
    }

    void RenderModule::init()
    {
        m_renderer = std::make_unique<AikoRenderer>(*m_assetManager->getManager(), m_assetManager->getManager());
        m_renderer->init();
    }

    void RenderModule::update()
    {
        BaseModule::update();
    }

    void RenderModule::beginFrame()
    {
        m_instances.clear();
        m_renderer->beginFrame();
    }

    void RenderModule::endFrame()
    {
        AIKO_ASSERT(m_mainCamera != nullptr, "Main camera not set. Forgot to call set Main camera?");
        m_renderer->render(*m_mainCamera);
        m_renderer->endFrame();
        m_instances.clear();
    }

    void RenderModule::dispose()
    {
        m_renderer->dispose();
    }

}
