#include "opengl_2d_renderer.h"

#include "core/libs.h"

#include "components/transform_component.h"

#include "shared/math.h"
#include "models/camera.h"
#include "constants.h"

namespace aiko
{

    constexpr float s_rotation = 0.0f;
    constexpr float s_zoom = 4.0f;

    Opengl2DRenderer::Opengl2DRenderer(RenderModule* renderer)
        : RenderContext2D(renderer)
    {
    }

    void Opengl2DRenderer::init()
    {
        AIKO_NOT_IMPLEMENTED
    }

    void Opengl2DRenderer::beginFrame()
    {

    }

    void Opengl2DRenderer::endFrame()
    {
        AIKO_NOT_IMPLEMENTED
    }

    void Opengl2DRenderer::dispose()
    {
    }

    void Opengl2DRenderer::drawRectangle(Camera* cam, vec2 pos, vec2 size, Color color)
    {
        AIKO_NOT_IMPLEMENTED
    }

    void Opengl2DRenderer::drawRectangle(Camera* cam, vec2 pos, vec2 size, std::array<Color, 4> colors)
    {
        AIKO_NOT_IMPLEMENTED
    }

    void Opengl2DRenderer::drawAbsRotation(Camera* cam, vec3 pos, vec2 size, const texture::Texture texture, const std::array<Color, 4> colors, const vec2 origin, const float rotationDegrees, const vec4 textureCoords)
    {
        AIKO_NOT_IMPLEMENTED
    }

    vec2 Opengl2DRenderer::positionToScreenCoords(vec3 pos, vec2 screenSize)
    {
        return { (pos.x / screenSize.x) * 2 - 1, -((-pos.y / screenSize.y) * 2 - 1) };
    }

    vec3 Opengl2DRenderer::rotateAroundPoint(vec3 vec, vec3 point, float degrees)
    {
        point.y = -point.y;
        float a = math::radians(degrees);
        float s = sinf(a);
        float c = cosf(a);
        vec.x -= point.x;
        vec.y -= point.y;
        float newx = vec.x * c - vec.y * s;
        float newy = vec.x * s + vec.y * c;
        // translate point back:
        vec.x = newx + point.x;
        vec.y = newy + point.y;
        return vec;
    }

    vec3 Opengl2DRenderer::scaleAroundPoint(vec3 vec, vec3 point, float scale)
    {
        vec = (vec - point) * scale + point;
        return vec;
    }

}