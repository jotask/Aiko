#include "application/application.h"

#include <time/time.h>
#include <aiko_renderer.h>
#include <input/inputs_types.h>

#include "aiko.h"
#include "systems/camera_system.h"
#include "systems/asset_system.h"
#include "systems/render_system.h"
#include "systems/input_system.h"
#include "systems/entity_component_system.h"

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
        AIKO_NOT_IMPLEMENTED
        // auto screen = AikoRenderer::it().getScreenFbo();
        //return screen->getViewportSize();
        return {};
    }

    float Application::getAspectRatio() const
    {
        auto target = getViewportSize();
        return target.x / target.x;
    }

    AssetSystem* Application::getAssetSystem() const
    {
        static auto* as = m_aiko->getSystem<AssetSystem>();
        return as;
    }

    InputSystem* Application::getInputSystem() const
    {
        static auto* is = m_aiko->getSystem<InputSystem>();
        return is;
    }

    RenderSystem* Application::getRenderSystem() const
    {
        static auto* rs = m_aiko->getSystem<RenderSystem>();
        return rs;
    }

    Camera* Application::getMainCamera()
    {
        auto cs = m_aiko->getSystem<CameraSystem>();
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

    GameObject* Application::Instantiate(char* name)
    {
        EntityComponentSystem* ecs = m_aiko->getSystem<EntityComponentSystem>();
        GameObject* obj = ecs->createGameObject(name).get();
        return obj;
    }

    GameObject* Application::Instantiate(GameObject* parent, char* name)
    {
        EntityComponentSystem* ecs = m_aiko->getSystem<EntityComponentSystem>();
        GameObject* obj = ecs->createGameObject(parent, name).get();
        return obj;
    }

    void Application::drawText(string str, float x, float y)
    {
        RenderSystem* renderSystem = getRenderSystem();
        renderSystem->renderText(str, x, y);
    }

}
