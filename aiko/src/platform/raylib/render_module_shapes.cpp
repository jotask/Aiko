#ifdef AIKO_BUILD_RAYLIB
#include "modules/render_module.h"

#include "core/libs.h"
#include "raylib_utils.h"

namespace aiko
{

    void RenderModule::drawPixel(int x, int y, aiko::Color color)
    {
        ::Color c = raylib::utils::toRaylibColor(color);
        ::DrawPixel(x, y , c);
    }

    void RenderModule::drawPixel(vec2 pos, Color color)
    {
        ::Vector2 p = raylib::utils::toRaylibVector2(pos);
        ::Color c = raylib::utils::toRaylibColor(color);
        ::DrawPixelV(p, c);
    }

    void RenderModule::drawLine(int startPosX, int startPosY, int endPosX, int endPosY, Color color)
    {
        ::Color c = raylib::utils::toRaylibColor(color);
        ::DrawLine(startPosX, startPosY, endPosX, endPosY, c);
    }

    void RenderModule::drawLine(vec2 startPos, vec2 endPos, Color color)
    {
        ::Vector2 s = raylib::utils::toRaylibVector2(startPos);
        ::Vector2 e = raylib::utils::toRaylibVector2(endPos);
        ::Color c = raylib::utils::toRaylibColor(color);
        ::DrawLineV( s, e , c);
    }

    void RenderModule::drawCircle(int centerX, int centerY, float radius, Color color)
    {
        ::Color c = raylib::utils::toRaylibColor(color);
        ::DrawCircle(centerX, centerY, radius, c);
    }

    void RenderModule::drawCircle(vec2 center, float radius, Color color)
    {
        ::Vector2 p = raylib::utils::toRaylibVector2(center);
        ::Color c = raylib::utils::toRaylibColor(color);
        ::DrawCircleV(p, radius, c);
    }

    void RenderModule::drawCircleLines(int centerX, int centerY, float radius, Color color)
    {
        ::Color c = raylib::utils::toRaylibColor(color);
        ::DrawCircleLines(centerX, centerY, radius, c);
    }

    void RenderModule::drawCircleLines(vec2 center, float radius, Color color)
    {
        ::Vector2 p = raylib::utils::toRaylibVector2(center);
        ::Color c = raylib::utils::toRaylibColor(color);
        ::DrawCircleLinesV(p, radius, c);
    }

    void RenderModule::drawEllipse(int centerX, int centerY, float radiusH, float radiusV, Color color)
    {
        ::Color c = raylib::utils::toRaylibColor(color);
        ::DrawEllipse(centerX, centerY, radiusH, radiusV, c);
    }

    void RenderModule::drawEllipseLines(int centerX, int centerY, float radiusH, float radiusV, Color color)
    {
        ::Color c = raylib::utils::toRaylibColor(color);
        ::DrawEllipseLines(centerX, centerY, radiusH, radiusV, c);
    }

    void RenderModule::drawRing(vec2 center, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, Color color)
    {
        ::Vector2 p = raylib::utils::toRaylibVector2(center);
        ::Color c = raylib::utils::toRaylibColor(color);
        ::DrawRing(p, innerRadius, outerRadius, startAngle, endAngle, segments, c);
    }

    void RenderModule::drawRingLines(vec2 center, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, Color color)
    {
        ::Vector2 p = raylib::utils::toRaylibVector2(center);
        ::Color c = raylib::utils::toRaylibColor(color);
        ::DrawRingLines(p, innerRadius, outerRadius, startAngle, endAngle, segments, c);
    }

    void RenderModule::drawRectangle(int posX, int posY, int width, int height, Color color)
    {
        ::Color c = raylib::utils::toRaylibColor(color);
        ::DrawRectangle(posX, posY, width, height, c);
    }

    void RenderModule::drawRectangle(vec2 position, vec2 size, Color color)
    {
        ::Vector2 p = raylib::utils::toRaylibVector2(position);
        ::Vector2 s = raylib::utils::toRaylibVector2(size);
        ::Color c = raylib::utils::toRaylibColor(color);
        ::DrawRectangleV(p, s, c);
    }

    void RenderModule::drawRectangleRec(Rectangle rec, Color color)
    {
        ::Rectangle r = raylib::utils::toRaylibRectangle(rec);
        ::Color c = raylib::utils::toRaylibColor(color);
        ::DrawRectangleRec(r, c);
    }

    void RenderModule::drawRectangleLines(int posX, int posY, int width, int height, Color color)
    {
        ::Color c = raylib::utils::toRaylibColor(color);
        ::DrawRectangleLines(posX, posY, width, height, c);
    }

    void RenderModule::drawRectangleLinesEx(aiko::Rectangle rec, float lineThick, Color color)
    {
        ::Rectangle r = raylib::utils::toRaylibRectangle(rec);
        ::Color c = raylib::utils::toRaylibColor(color);
        ::DrawRectangleLinesEx(r, lineThick, c);
    }

    void RenderModule::drawRectangleRounded(Rectangle rec, float roundness, int segments, Color color)
    {
        ::Rectangle r = raylib::utils::toRaylibRectangle(rec);
        ::Color c = raylib::utils::toRaylibColor(color);
        ::DrawRectangleRounded(r, roundness, segments, c);
    }

    void RenderModule::drawRectangleRoundedLines(Rectangle rec, float roundness, int segments, float lineThick, Color color)
    {
        ::Rectangle r = raylib::utils::toRaylibRectangle(rec);
        ::Color c = raylib::utils::toRaylibColor(color);
        ::DrawRectangleRoundedLines(r, roundness, segments, lineThick, c);
    }

    void RenderModule::drawTriangle(vec2 v1, vec2 v2, vec2 v3, Color color)
    {
        ::Vector2 p1 = raylib::utils::toRaylibVector2(v1);
        ::Vector2 p2 = raylib::utils::toRaylibVector2(v2);
        ::Vector2 p3 = raylib::utils::toRaylibVector2(v3);
        ::Color c = raylib::utils::toRaylibColor(color);
        ::DrawTriangle(p1, p2, p3, c);
    }

    void RenderModule::drawTriangleLines(vec2 v1, vec2 v2, vec2 v3, Color color)
    {
        ::Vector2 p1 = raylib::utils::toRaylibVector2(v1);
        ::Vector2 p2 = raylib::utils::toRaylibVector2(v2);
        ::Vector2 p3 = raylib::utils::toRaylibVector2(v3);
        ::Color c = raylib::utils::toRaylibColor(color);
        ::DrawTriangleLines(p1, p2, p3, c);
    }

    void RenderModule::drawPoly(vec2 center, int sides, float radius, float rotation, Color color)
    {
        ::Vector2 p = raylib::utils::toRaylibVector2(center);
        ::Color c = raylib::utils::toRaylibColor(color);
        ::DrawPoly(p, sides, radius, rotation, c);
    }

    void RenderModule::drawPolyLines(vec2 center, int sides, float radius, float rotation, Color color)
    {
        ::Vector2 p = raylib::utils::toRaylibVector2(center);
        ::Color c = raylib::utils::toRaylibColor(color);
        ::DrawPolyLines(p, sides, radius, rotation, c);
    }

    void RenderModule::drawPolyLinesEx(vec2 center, int sides, float radius, float rotation, float lineThick, Color color)
    {
        ::Vector2 p = raylib::utils::toRaylibVector2(center);
        ::Color c = raylib::utils::toRaylibColor(color);
        ::DrawPolyLinesEx(p, sides, radius, rotation, lineThick, c);
    }

}
#endif
