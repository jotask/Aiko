#include "application/application.h"

#include "aiko.h"
#include "systems/camera_system.h"
#include "systems/asset_system.h"
#include "systems/render_system.h"
#include "systems/input_system.h"
#include "models/shader.h"
#include "models/time.h"

#include "core/libs.h"

namespace aiko
{

    Application::Application()
        : m_aiko( std::make_unique<Aiko>( this ) )
    {

    }

    vec2 Application::getViewportSize() const
    {
        auto target = getTargetTexture()->texture;
        return { static_cast<float>(target.width) , static_cast<float>(target.height) };
    }

    float Application::getAspectRatio() const
    {
        auto target = getTargetTexture()->texture;
        return static_cast<float>(target.width) / static_cast<float>(target.height);
    }

    texture::RenderTexture2D* Application::getTargetTexture() const
    {
        static auto rs = m_aiko->getSystem<RenderSystem>();
        return rs->getTargetTexture();
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
        static auto cs = m_aiko->getSystem<CameraSystem>();
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

    aiko::asset::Shader* Application::loadShader(const char* vs, const char* fs)
    {
        auto* assetSystem = getAssetSystem();
        // return assetSystem->loadShader(vs, fs);
        return nullptr;
    }

    void Application::unloadShader(aiko::asset::Shader* shader)
    {
        // auto* assetSystem = getAssetSystem();
        // assetSystem->unload(shader->getID());
    }

    GameObject* Application::createGameObject(char* name)
    {
        return m_aiko->createGameObject(name);
    }

}
