#include "render_module.h"

#include "aiko.h"
#include "assets_manager_module.h"
#include "display/display_manager.h"
#include "models/camera.h"
#include "models/mesh_factory.h"
#include "modules/module_connector.h"
#include "time/time.h"
#include "types/builtin_shaders.h"

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
        BIND_MODULE_REQUIRED(AssetsManagerModule, moduleConnector, m_assetManager);
    }

    void RenderModule::init()
    {
        AssetManager* assetManager = m_assetManager->getManager();

        const AssetId passthroughShaderId = assetManager->registerShader(renderer::BuiltinShader::Passthrough);
        assetManager->loadShaderAsset(passthroughShaderId);

        m_renderer = std::make_unique<AikoRenderer>(*assetManager, passthroughShaderId);
        m_renderer->init(getAiko()->getConfig().renderer);
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
