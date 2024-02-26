#include "application/application.h"

#include <aiko.h>
#include "systems/camera_system.h"
#include "systems/render_system.h"

namespace aiko
{

    Application::Application()
        : m_aiko( std::make_unique<Aiko>( this ) )
    {

    }


    vec2 Application::getDisplaySize() const
    {
        static auto cs = m_aiko->getSystem<CameraSystem>();
        return cs->getDisplaySize();
    }

    RenderTexture2D* Application::getTargetTexture() const
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

}
