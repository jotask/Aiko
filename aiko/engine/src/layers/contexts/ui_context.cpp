#include "ui_context.h"

#include "systems/render_system.h"
#include "systems/system_connector.h"

namespace aiko
{

    UIContext::UIContext(SystemConnector& connector)
    {
        m_renderSystem = connector.find<RenderSystem>();
        AIKO_ASSERT(m_renderSystem != nullptr, "Required system RenderSystem not found");
    }

    void UIContext::rect(const vec2& position, const vec2& size, Color color)
    {
        m_renderSystem->drawUiRect(position, size, color);
    }

    void UIContext::image(AssetId textureId, const vec2& position, const vec2& size, Color tint)
    {
        m_renderSystem->drawUiImage(textureId, position, size, tint);
    }
}
