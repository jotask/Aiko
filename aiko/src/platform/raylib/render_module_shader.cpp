#ifdef AIKO_BUILD_RAYLIB
#include "modules/render_module.h"

#include "core/libs.h"
#include "raylib_utils.h"

namespace aiko
{

    void RenderModule::drawTexture(texture::Texture texture, int posX, int posY, Color tint)
    {
        ::Texture2D t = raylib::utils::toRaylibTexture(texture);
        ::Color c = raylib::utils::toRaylibColor(tint);
        ::DrawTexture(t, posX, posY, c);
    }

    void RenderModule::drawTextureV(texture::Texture texture, vec2 position, Color tint)
    {
        ::Texture2D t = raylib::utils::toRaylibTexture(texture);
        ::Vector2 p = raylib::utils::toRaylibVector2(position);
        ::Color c = raylib::utils::toRaylibColor(tint);
        ::DrawTextureV(t, p, c);
    }

    void RenderModule::drawTextureEx(texture::Texture texture, vec2 position, float rotation, float scale, Color tint)
    {
        ::Texture2D t = raylib::utils::toRaylibTexture(texture);
        ::Vector2 p = raylib::utils::toRaylibVector2(position);
        ::Color c = raylib::utils::toRaylibColor(tint);
        ::DrawTextureEx(t, p, rotation, scale, c);
    }

    void RenderModule::drawTextureRec(texture::Texture texture, Rectangle source, vec2 position, Color tint)
    {
        ::Texture2D t = raylib::utils::toRaylibTexture(texture);
        ::Rectangle r = raylib::utils::toRaylibRectangle(source);
        ::Vector2 p = raylib::utils::toRaylibVector2(position);
        ::Color c = raylib::utils::toRaylibColor(tint);
        ::DrawTextureRec(t, r, p ,c);
    }

    void RenderModule::drawTexturePro(texture::Texture texture, Rectangle source, Rectangle dest, vec2 origin, float rotation, Color tint)
    {
        ::Texture2D t = raylib::utils::toRaylibTexture(texture);
        ::Rectangle r = raylib::utils::toRaylibRectangle(source);
        ::Rectangle d = raylib::utils::toRaylibRectangle(dest);
        ::Vector2 p = raylib::utils::toRaylibVector2(origin);
        ::Color c = raylib::utils::toRaylibColor(tint);
        ::DrawTexturePro(t, r, d, p, rotation, c);
    }

}
#endif
