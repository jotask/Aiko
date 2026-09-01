#include "application/application.h"

#include "aiko.h"
#include "systems/input_system.h"
#include "systems/scene_system.h"
#include "systems/system_connector.h"

#include <input/inputs_types.h>
#include <time/time.h>

#include <aiko_renderer.h>

namespace aiko
{

    Application::Application()
        : m_aiko(std::make_unique<Aiko>(this))
    {

    }

    Application::Application(AikoConfig cfg)
        : m_aiko(std::make_unique<Aiko>(this, cfg))
    {

    }

    vec2 Application::getViewportSize() const
    {
        AIKO_NOT_IMPLEMENTED;
        // auto screen = AikoRenderer::it().getScreenFbo();
        //return screen->getViewportSize();
        return {};
    }

    float Application::getAspectRatio() const
    {
        auto target = getViewportSize();
        return target.x / target.x;
    }

    Camera* Application::getMainCamera()
    {
        return m_sceneSystem->getMainCamera();
    }

    void Application::run()
    {
        m_aiko->run();
    }

    float Application::getlDeltaTime() const
    {
        return Time::it().getDeltaTime();
    }

    bool Application::isKeyPressed(Key key) const
    {
        return m_inputSystem->isKeyPressed(key);
    }

    bool Application::isKeyJustPressed(Key key) const
    {
        return m_inputSystem->isKeyJustPressed(key);
    }

    vec2 Application::getMousePosition() const
    {
        return m_inputSystem->getMousePosition();
    }

    bool Application::isMouseButtonPressed(MouseButton button) const
    {
        return m_inputSystem->isMouseButtonPressed(button);
    }

    void Application::registerSystems(SystemRegistry& registry)
    {
        for (auto& layer : m_layers)
        {
            layer->registerSystems(registry);
        }
    }

    void Application::init()
    {
        for (auto& layer : m_layers)
        {
            layer->init();
        }
    }

    void Application::update()
    {
        for (auto& layer : m_layers)
        {
            layer->update();
        }
    }

    void Application::render()
    {
        for (auto& layer : m_layers)
        {
            layer->render();
        }
    }

    void Application::dispose()
    {
        for (auto& layer : m_layers)
        {
            layer->dispose();
        }
    }

    GameObject* Application::Instantiate(string name)
    {
        GameObject* obj = m_sceneSystem->createGameObject(name).get();
        return obj;
    }

    GameObject* Application::Instantiate(GameObject* parent, string name)
    {
        GameObject* obj = m_sceneSystem->createGameObject(parent, name).get();
        return obj;
    }

    void Application::setActiveCamera(GameObject* obj)
    {
        m_sceneSystem->setActiveCamera(obj);
    }

    void Application::pushLayer(AikoUPtr<Layer> layer)
    {
        layer->app = this;
        m_layers.pushLayer(std::move(layer));
    }

    void Application::pushOverlay(AikoUPtr<Layer> layer)
    {
        layer->app = this;
        m_layers.pushOverlay(std::move(layer));
    }

    void Application::connect(SystemConnector* connector)
    {
        BIND_SYSTEM_REQUIRED(InputSystem, connector, m_inputSystem);
        BIND_SYSTEM_REQUIRED(SceneSystem, connector, m_sceneSystem);
        for (auto& layer : m_layers)
        {
            layer->connect(connector);
        }
    }

    void Application::onEvent(Event& e)
    {
        for (auto it = m_layers.rbegin(); it != m_layers.rend(); ++it)
        {
            (*it)->onEvent(e);
            if (e.handled == true)
            {
                break;
            }
        }
    }
}
