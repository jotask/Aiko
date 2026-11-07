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

    void UIContext::line(const vec2& start, const vec2& end, float thickness, Color color)
    {
        m_renderSystem->drawUiLine(start, end, thickness, color);
    }

    void UIContext::rect(const vec2& position, const vec2& size, Color color)
    {
        m_renderSystem->drawUiRect(position, size, color);
    }

    void UIContext::image(AssetId textureId, const vec2& position, const vec2& size, Color tint, const SamplerState& sampler)
    {
        m_renderSystem->drawUiImage(textureId, position, size, tint, 0.0f, 0.0f, WHITE, sampler);
    }

    void UIContext::image(AssetId textureId, const TextureRegion& region, const vec2& position, const vec2& size, Color tint, const SamplerState& sampler)
    {
        m_renderSystem->drawUiImage(textureId, region, position, size, tint, 0.0f, 0.0f, WHITE, sampler);
    }

    void UIContext::text(const Font& font, string_view text, const vec2& position, float fontSize, Color color)
    {
        m_renderSystem->drawUiText(font, text, position, fontSize, color);
    }

    void UIContext::pushClipRect(const vec2& position, const vec2& size)
    {
        m_renderSystem->pushUiClipRect(position, size);
    }

    void UIContext::popClipRect()
    {
        m_renderSystem->popUiClipRect();
    }
}
