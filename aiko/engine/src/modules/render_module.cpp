#include "render_module.h"

#include "aiko.h"
#include "assets_manager_module.h"
#include "modules/display_module.h"
#include "models/camera.h"
#include "modules/module_connector.h"

#include <aiko_renderer.h>

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

    void RenderModule::submitLights(const AmbientLight& ambient, const vector<LightData>& data)
    {
        m_renderer->submit(ambient, data);
    }

    void RenderModule::connect(ModuleConnector* moduleConnector)
    {
        BIND_MODULE_REQUIRED(DisplayModule, moduleConnector, m_displayModule);
        BIND_MODULE_REQUIRED(AssetsManagerModule, moduleConnector, m_assetManager);
    }

    void RenderModule::init()
    {

        AssetManager* assetManager = m_assetManager->getManager();

        const ivec2 size = m_displayModule->getDisplaySize();
        const RenderSurfaceDesc surface =
        {
            .nativeWindowHandle = m_displayModule->getNativeWindow(),
            .width = static_cast<u32>(size.x),
            .height = static_cast<u32>(size.y),
        };

        m_renderer = std::make_unique<AikoRenderer>(*assetManager);
        m_renderer->init(getAiko()->getConfig().renderer, surface);
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

    void RenderModule::preDispose()
    {
        m_renderer->waitIdle();
    }

    void RenderModule::dispose()
    {
        m_renderer->dispose();
    }

}
