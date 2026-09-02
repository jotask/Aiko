#include "render_context.h"

#include "systems/render_system.h"
#include "systems/system_connector.h"

namespace aiko
{

    RenderContext::RenderContext(SystemConnector& connector)
    {
        m_renderSystem = connector.find<RenderSystem>();
        AIKO_ASSERT(m_renderSystem != nullptr, "Required system RenderSystem not found");
    }

    void RenderContext::drawRectangle(const vec3& position, const vec3& size)
    {
        m_renderSystem->renderRectangle(position, size);
    }

    void RenderContext::drawMesh(const Transform& transform, const Mesh& mesh, const Material& material)
    {
        m_renderSystem->render(transform, mesh, material);
    }

}
