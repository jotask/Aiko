#include "aiko_renderer.h"

#include "models/shader.h"
#include "models/mesh.h"

namespace aiko
{

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

    void AikoRenderer::setBackgroundColor(const Color color)
    {
        m_background_color = color;
    }

    void AikoRenderer::initScreenFbo()
    {
        // Postprocessing shader
        m_passThrough.load("posprocessing");
        AIKO_ASSERT(m_passThrough.isvalid(), "Processing shader invalid!");
    }
}
