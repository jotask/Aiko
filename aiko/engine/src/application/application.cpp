#include "application/application.h"

#include <time/time.h>
#include <aiko_renderer.h>
#include <input/inputs_types.h>

#include "aiko.h"
#include "systems/render_system.h"
#include "systems/input_system.h"
#include "systems/scene_system.h"

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

    InputSystem* Application::getInputSystem() const
    {
        return m_aiko->getSystem<InputSystem>();
    }

    RenderSystem* Application::getRenderSystem() const
    {
        return m_aiko->getSystem<RenderSystem>();
    }

    Camera* Application::getMainCamera()
    {
        auto cs = m_aiko->getSystem<SceneSystem>();
        return cs->getMainCamera();
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
        InputSystem* inputSystem = getInputSystem();
        return inputSystem->isKeyPressed(key);
    }

    bool Application::isKeyJustPressed(Key key) const
    {
        InputSystem* inputSystem = getInputSystem();
        return inputSystem->isKeyJustPressed(key);
    }

    vec2 Application::getMousePosition() const
    {
        InputSystem* inputSystem = getInputSystem();
        return inputSystem->getMousePosition();
    }

    bool Application::isMouseButtonPressed(MouseButton button) const
    {
        InputSystem* inputSystem = getInputSystem();
        return inputSystem->isMouseButtonPressed(button);
    }

    void Application::registerSystems()
    {
        for (auto& layer : m_layers)
        {
            layer->registerSystems();
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
        SceneSystem* ecs = m_aiko->getSystem<SceneSystem>();
        GameObject* obj = ecs->createGameObject(name).get();
        return obj;
    }

    GameObject* Application::Instantiate(GameObject* parent, string name)
    {
        SceneSystem* ecs = m_aiko->getSystem<SceneSystem>();
        GameObject* obj = ecs->createGameObject(parent, name).get();
        return obj;
    }

    void Application::setActiveCamera(GameObject* obj)
    {
        SceneSystem* ss = m_aiko->getSystem<SceneSystem>();
        AIKO_ASSERT(ss != nullptr, "Scene system not found");
        ss->setActiveCamera(obj);
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
