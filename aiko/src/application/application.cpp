#include "application/application.h"

#include <aiko.h>
#include "systems/camera_system.h"
#include "systems/render_system.h"

#include "core/raylib_utils.h"

#include <core/libs.h>

namespace aiko
{

    Application::Application()
        : m_aiko( std::make_unique<Aiko>( this ) )
    {

    }


    ivec2 Application::getDisplaySize() const
    {
        static auto cs = m_aiko->getSystem<CameraSystem>();
        return cs->getDisplaySize();
    }

    texture::RenderTexture2D* Application::getTargetTexture() const
    {
        static auto rs = m_aiko->getSystem<RenderSystem>();
        return rs->getTargetTexture();
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
