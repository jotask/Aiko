#pragma once

#include <math/math.h>
#include <models/texture_region.h>
#include <types/color.h>

namespace aiko
{

    class SystemConnector;
    class RenderSystem;
    class Font;

    class UIContext
    {
    public:

        void rect(const vec2& position, const vec2& size, Color color);
        void image(AssetId textureId, const vec2& position, const vec2& size, Color tint = WHITE);
        void image(AssetId textureId, const TextureRegion& region, const vec2& position, const vec2& size, Color tint = WHITE);
        void text(const Font& font, string_view text, const vec2& position, float fontSize, Color color = WHITE);

        void pushClipRect(const vec2& position, const vec2& size);
        void popClipRect();

    private:

        friend class LayerContext;

        explicit UIContext(SystemConnector& connector);

        RenderSystem* m_renderSystem = nullptr;
    };

}