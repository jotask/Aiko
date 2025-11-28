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
        //return m_displayModule->getCurrentDisplay().getDisplaySize();
        AIKO_NOT_IMPLEMENTED;
        return {0};
    }

    void AikoRenderer::setBackgroundColor(const Color color)
    {
        background_color = color;
    }

}
