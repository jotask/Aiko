#include "render_module.h"

#include <events/events.hpp>

#include "models/shader.h"
#include "models/mesh.h"

namespace aiko
{
    AikoRenderer::AikoRenderer()
    {
    }

    void AikoRenderer::init()
    {
        const AikoConfig cfg = getAiko()->getConfig();
        background_color = cfg.background_color;
        EventSystem::it().bind<WindowResizeEvent>(this, &AikoRenderer::onWindowResize);
    }

    void AikoRenderer::postInit()
    {

    }

    void AikoRenderer::beginFrame()
    {

    }

    void AikoRenderer::endFrame()
    {

    }

    void AikoRenderer::dispose()
    {

    }

    ivec2 AikoRenderer::getDisplaySize()
    {
        return m_displayModule->getCurrentDisplay().getDisplaySize();
    }

    void AikoRenderer::onWindowResize(Event& event)
    {
        const auto& msg = static_cast<const WindowResizeEvent&>(event);
        m_displayModule->getCurrentDisplay().setWindowSize(msg.width, msg.height);
    }

}
