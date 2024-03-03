#include "application/application.h"

#include <aiko.h>
#include "systems/camera_system.h"
#include "systems/render_system.h"

// FIXME Delete me
#include "platform/raylib/raylib_utils.h"
#include <core/libs.h>

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

    RenderSystem* Application::getRenderSystem()
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
        return ::GetFrameTime();
    }

    bool Application::isKeyPressed(Key key) const
    {
        return ::IsKeyPressed((KeyboardKey)key);
    }

    vec2 Application::getMousePosition() const
    {
        return raylib::utils::toVec2(::GetMousePosition());
    }

    bool Application::isMouseButtonPressed(MouseButton button) const
    {
        return ::IsMouseButtonPressed((::MouseButton)button);
    }

}
