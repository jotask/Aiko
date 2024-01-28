#include "renderer_component.h"

#include "modules/renderer/render_module.h"

namespace aiko
{
    
    RendererComponent::RendererComponent(RenderModule* renderModule)
        : m_renderModule(renderModule)
    {
    
    }

    vec2 RendererComponent::getDisplaySize()
    {
        return m_renderModule->getDisplaySize();
    }

}
