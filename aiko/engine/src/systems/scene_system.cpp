#include "scene_system.h"

#include "systems/entity_component_system.h"
#include "modules/render_module.h"

#include "modules/module_connector.h"
#include "systems/system_connector.h"

namespace aiko
{
    SceneSystem::SceneSystem()
        : m_entitySystem(nullptr)
        , m_renderModule(nullptr)
    {

    }

    void SceneSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        BIND_MODULE_REQUIRED(RenderModule, moduleConnector, m_renderModule);
        BIND_SYSTEM_REQUIRED(EntityComponentSystem, systemConnector, m_entitySystem);
    }

    void SceneSystem::render()
    {
        Scene& scene = m_entitySystem->scene();
        SceneView view = m_sceneViewBuilder.build(scene);
        if(view.camera != nullptr)
        {
            AikoRenderer::it().submit(view.ambientLight, view.lights);
            m_renderModule->setMainCamera(view.camera);
        }
        m_sceneRenderer.render(scene);
    }
}
