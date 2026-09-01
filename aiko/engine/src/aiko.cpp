#include "aiko.h"

#include <events/events.hpp>
#include <logger/logger.h>
#include <display/display_events.hpp>
#include <time/time.h>

#include "modules/module_connector.h"
#include "systems/system_connector.h"

#include "application/application.h"

// Modules
#include "modules/display_module.h"
#include "modules/render_module.h"
#include "modules/input_module.h"

// Systems
#include "modules/assets_manager_module.h"
#include "systems/asset_binding_system.h"
#include "systems/asset_system.h"
#include "systems/camera_system.h"
#include "systems/compute_system.h"
#include "systems/input_system.h"
#include "systems/particle_system.h"
#include "systems/physics_system.h"
#include "systems/render_system.h"
#include "systems/scene_system.h"
#include "systems/system_registry.h"

namespace aiko
{

    Aiko::Aiko(Application* app) : Aiko(app, AikoConfig()) { }

    Aiko::Aiko(Application * app,  AikoConfig cfg)
        : m_application(app)
        , m_shouldStop(false)
        , cfg(cfg)
    {
        logger::Log::init();
        EventSystem::it().bind<WindowCloseEvent>(this, &Aiko::onWindowClose);
    }

    Aiko::~Aiko()
    {
    }

    void Aiko::onWindowClose(WindowCloseEvent& event)
    {
        m_shouldStop = true;
    }

    void Aiko::close()
    {
        m_shouldStop = true;
    }

    void Aiko::run()
    {
        init();
        while (m_shouldStop == false)
        {
            Time::it().update();
            update();
            render();
        }
        dispose();
    }

    void Aiko::init()
    {

        // Modules
        m_modules.emplace_back(std::make_unique<DisplayModule>(this));
        m_modules.emplace_back(std::make_unique<RenderModule>(this));
        m_modules.emplace_back(std::make_unique<InputModule>(this));
        m_modules.emplace_back(std::make_unique<AssetsManagerModule>(this));

        ModuleConnector moduleConnector(m_modules);

        for (auto&& module : m_modules) module->connect(&moduleConnector);

        for (auto&& module : m_modules) module->preInit();
        for (auto&& module : m_modules) module->init();
        for (auto&& module : m_modules) module->postInit();

        // Systems
        SystemRegistry systemRegistry(m_systems);

        systemRegistry.add<SceneSystem>();
        systemRegistry.add<RenderSystem>();
        systemRegistry.add<AssetSystem>();
        systemRegistry.add<ComputeSystem>();
        systemRegistry.add<InputSystem>();
        systemRegistry.add<CameraSystem>();
        systemRegistry.add<AssetBindingSystem>();
        systemRegistry.add<PhysicsSystem>();
        systemRegistry.add<ParticleSystem>();

        m_application->registerSystems(systemRegistry);

        SystemConnector systemConnector(m_systems);
        for (auto&& system : m_systems) system->connect(&moduleConnector, &systemConnector);
        for (auto&& system : m_systems) system->init();

        m_application->connect(&systemConnector);
        m_application->init();

    }

    void Aiko::update()
    {
        for (auto&& module : m_modules) module->preUpdate();
        for (auto&& module : m_modules) module->update();
        for (auto&& system : m_systems) system->update();
        m_application->update();
        for (auto&& module : m_modules) module->postUpdate();
    }

    void Aiko::render()
    {
        for (auto&& module : m_modules) module->beginFrame();
        for (auto&& module : m_modules) module->preRender();
        for (auto&& module : m_modules) module->render();
        for (auto&& system : m_systems) system->render();
        m_application->render();
        for (auto&& module : m_modules) module->postRender();
        for (auto&& module : m_modules) module->endFrame();
    }

    void Aiko::dispose()
    {
        for (auto&& module : m_modules) module->preDispose();
        m_application->dispose();
        for (auto&& system : m_systems) system->dispose();
        for (auto&& module : m_modules) module->dispose();
    }

}
