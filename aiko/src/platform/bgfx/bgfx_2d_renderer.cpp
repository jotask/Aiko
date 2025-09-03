#ifdef AIKO_BGFX

#include "bgfx_2d_renderer.h"

#include "core/libs.h"

#include "components/transform_component.h"

#include "shared/math.h"
#include "models/camera.h"
#include "constants.h"

namespace aiko
{

    constexpr float s_rotation = 0.0f;
    constexpr float s_zoom = 4.0f;

    Bgfx2DRenderer::Bgfx2DRenderer(RenderModule* renderer)
        : RenderContext2D(renderer)
    {
    }

    void Bgfx2DRenderer::init()
    {
        AIKO_NOT_IMPLEMENTED
    }

    void Bgfx2DRenderer::beginFrame()
    {

    }

    void Bgfx2DRenderer::endFrame()
    {
        AIKO_NOT_IMPLEMENTED
    }

    void Bgfx2DRenderer::dispose()
    {

    }

    void Bgfx2DRenderer::drawRectangle(Camera* cam, vec2 pos, vec2 size, Color color)
    {
        vec2 newOrigin = pos;
        std::array<Color, 4> colors;
        std::fill(colors.begin(), colors.end(), color);
        drawAbsRotation(cam, pos, size, objs.defaultTexture, colors, newOrigin, 0);
        objs.projection = cam->getProjectionMatrix();
        objs.view = cam->getViewMatrix();
    }

    void Bgfx2DRenderer::drawRectangle(Camera* cam, vec2 pos, vec2 size, std::array<Color, 4> colors)
    {
        vec2 newOrigin;
        drawAbsRotation(cam, pos, size, objs.defaultTexture, colors, newOrigin, 0);
    }

    void Bgfx2DRenderer::drawAbsRotation(Camera* cam, vec3 pos, vec2 size, const texture::Texture texture, const std::array<Color, 4> colors, const vec2 origin, const float rotationDegrees, const vec4 textureCoords)
    {
        texture::Texture textureCopy = texture;

        if (textureCopy.id == 0)
        {
            Log::error("Invalid texture");
            textureCopy = objs.defaultTexture;
        }

        // We need to flip texture_transforms.y
        const float transformsY = pos.y * -1;

        vec3 v1 = { pos.x,          transformsY          , pos.z };
        vec3 v2 = { pos.x,          transformsY - size.y , pos.z }; // Corrected size.x to size.y
        vec3 v3 = { pos.x + size.x, transformsY - size.y , pos.z }; // Corrected size.y to size.x
        vec3 v4 = { pos.x + size.x, transformsY          , pos.z }; // Corrected size.y to size.x

        // Apply rotations
        if (rotationDegrees != 0)
        {
            v1 = rotateAroundPoint(v1, origin, rotationDegrees);
            v2 = rotateAroundPoint(v2, origin, rotationDegrees);
            v3 = rotateAroundPoint(v3, origin, rotationDegrees);
            v4 = rotateAroundPoint(v4, origin, rotationDegrees);
        }

        // Apply camera transformations
        v1.x -= cam->position.x;
        v1.y += cam->position.y;
        v2.x -= cam->position.x;
        v2.y += cam->position.y;
        v3.x -= cam->position.x;
        v3.y += cam->position.y;
        v4.x -= cam->position.x;
        v4.y += cam->position.y;

        const auto screenSize = getRenderModule()->getDisplaySize();

        // Apply camera rotation
        if (s_rotation != 0)
        {
            vec2 cameraCenter = { screenSize.x / 2.0f, screenSize.y / 2.0f };
            v1 = rotateAroundPoint(v1, cameraCenter, s_rotation);
            v2 = rotateAroundPoint(v2, cameraCenter, s_rotation);
            v3 = rotateAroundPoint(v3, cameraCenter, s_rotation);
            v4 = rotateAroundPoint(v4, cameraCenter, s_rotation);
        }

        // Apply camera zoom
        if (s_zoom != 1)
        {
            vec2 cameraCenter = { cam->position.x , cam->position.y };
            v1 = scaleAroundPoint(v1, cameraCenter, s_zoom);
            v2 = scaleAroundPoint(v2, cameraCenter, s_zoom);
            v3 = scaleAroundPoint(v3, cameraCenter, s_zoom);
            v4 = scaleAroundPoint(v4, cameraCenter, s_zoom);
        }

        v1 = positionToScreenCoords(v1, screenSize);
        v2 = positionToScreenCoords(v2, screenSize);
        v3 = positionToScreenCoords(v3, screenSize);
        v4 = positionToScreenCoords(v4, screenSize);

        objs.spritePositions.push_back(v1);
        objs.spritePositions.push_back(v2);
        objs.spritePositions.push_back(v4);

        objs.spritePositions.push_back(v2);
        objs.spritePositions.push_back(v3);
        objs.spritePositions.push_back(v4);

        objs.spriteColors.push_back(colors[0]); // v1
        objs.spriteColors.push_back(colors[1]); // v2
        objs.spriteColors.push_back(colors[3]); // v4

        objs.spriteColors.push_back(colors[1]); // v2
        objs.spriteColors.push_back(colors[2]); // v3
        objs.spriteColors.push_back(colors[3]); // v4

        objs.texturePositions.push_back(vec2{ textureCoords.x, textureCoords.y }); //1
        objs.texturePositions.push_back(vec2{ textureCoords.x, textureCoords.w }); //2
        objs.texturePositions.push_back(vec2{ textureCoords.z, textureCoords.y }); //4
        objs.texturePositions.push_back(vec2{ textureCoords.x, textureCoords.w }); //2
        objs.texturePositions.push_back(vec2{ textureCoords.z, textureCoords.w }); //3
        objs.texturePositions.push_back(vec2{ textureCoords.z, textureCoords.y }); //4

        objs.spriteTextures.push_back(textureCopy);
    }

    vec2 Bgfx2DRenderer::positionToScreenCoords(vec3 pos, vec2 screenSize)
    {
        return { (pos.x / screenSize.x) * 2 - 1, -((-pos.y / screenSize.y) * 2 - 1) };
    }

    vec3 Bgfx2DRenderer::rotateAroundPoint(vec3 vec, vec3 point, float degrees)
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

    vec3 Bgfx2DRenderer::scaleAroundPoint(vec3 vec, vec3 point, float scale)
    {
        vec = (vec - point) * scale + point;
        return vec;
    }

}

#endif