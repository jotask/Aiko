#include "renderer_context.h"

namespace aiko
{
    RenderContext::RenderContext(RenderModule* renderer)
        : m_renderModule(renderer)
    {

    }
    RenderModule* RenderContext::getRenderModule()
    {
        return m_renderModule;
    }
}
