#include "aiko.h"


#include "modules/module_connector.h"
#include "systems/system_connector.h"

#include "application/application.h"

// Modules
#include "modules/display_module.h"
#include "modules/scene_module.h"
#include "modules/camera_module.h"
#include "modules/input_module.h"
#include "modules/renderer/render_module.h"

// Systems
#include "systems/entity_component_system.h"
#include "systems/camera_system.h"
#include "systems/render_system.h"

const AikoConfig default_aiko_cfg;

Aiko::Aiko(Application* app)
    : Aiko(app, default_aiko_cfg)
{

}

Aiko::Aiko(Application * app,  AikoConfig cfg)
    : m_application(app)
    , cfg(cfg)
    , m_displayModule(nullptr)
{

}

Aiko::~Aiko()
{
}

GameObject* Aiko::createGameObject(std::string name)
{
    static EntityComponentSystem* ecs = getSystem<EntityComponentSystem>().get();
    GameObject* obj = ecs->createGameObject(name).get();
    return obj;
}

void Aiko::run()
{
    init();
    m_application->init();
    while (m_displayModule->isOpen() == true)
    {
        update();
        m_application->update();
        render();
        m_application->render();
    }
    dispose();
}

void Aiko::init()
{

    // Modules
    m_modules.emplace_back(std::make_shared<DisplayModule>());
    m_modules.emplace_back(std::make_shared<SceneModule>());
    m_modules.emplace_back(std::make_shared<RenderModule>());
    m_modules.emplace_back(std::make_shared<InputModule>());
    m_modules.emplace_back(std::make_shared<CameraModule>());

    ModuleConnector moduleConnector(m_modules);

    // Temporal code
    {
        auto ptr = moduleConnector.find<DisplayModule>();
        m_displayModule = ptr.get();
    }

    for (auto&& module : m_modules) module->connect(&moduleConnector);

    for (auto&& module : m_modules) module->preInit();
    for (auto&& module : m_modules) module->init();
    for (auto&& module : m_modules) module->postInit();

    // Systems
    m_systems.emplace_back(std::make_shared<EntityComponentSystem>());
    m_systems.emplace_back(std::make_shared<RenderSystem>());
    m_systems.emplace_back(std::make_shared<CameraSystem>());

    SystemConnector systemConnector(m_systems);
    for (auto&& system : m_systems) system->aiko = this;
    for (auto&& system : m_systems) system->connect(&moduleConnector, &systemConnector);
    for (auto&& system : m_systems) system->init();

}

void Aiko::update()
{
    for (auto&& module : m_modules) module->preUpdate();
    for (auto&& module : m_modules) module->update();
    for (auto&& system : m_systems) system->update();
    for (auto&& module : m_modules) module->postUpdate();
}

void Aiko::render()
{
    for (auto&& module : m_modules) module->beginFrame();
    for (auto&& module : m_modules) module->preRender();
    for (auto&& module : m_modules) module->render();
    for (auto&& system : m_systems) system->render();
    for (auto&& module : m_modules) module->postRender();
    for (auto&& module : m_modules) module->endFrame();
}

void Aiko::dispose()
{
    for (auto&& module : m_modules) module->dispose();
}
