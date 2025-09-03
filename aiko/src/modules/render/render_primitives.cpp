#include "render_primitives.h"

#include "types/light_types.h"
#include "shared/math.h"
#include "shared/math_transform.h"
#include "systems/render_system.h"
#include "models/camera.h"
#include "core/libs.h"
#include "constants.h"

#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace aiko
{

    AikoPtr<Shader> Primitives::shader = nullptr;
    RenderSystem* Primitives::renderSystem = nullptr;

    unsigned int loadTexture(char const* path)
    {
        AIKO_NOT_IMPLEMENTED
    }

    void Primitives::init(RenderSystem* system)
    {
        AIKO_NOT_IMPLEMENTED
    }

    void Primitives::use()
    {
        AIKO_NOT_IMPLEMENTED
    }

    void Primitives::unuse()
    {
        AIKO_NOT_IMPLEMENTED
    }

    void Primitives::bindData(std::vector<float>& verts)
    {
        AIKO_NOT_IMPLEMENTED
    }

    void Primitives::calculateNormals(Data& data)
    {
        AIKO_NOT_IMPLEMENTED
    }

    void Primitives::calculateUvs(Data& data)
    {

        AIKO_NOT_IMPLEMENTED
    }

    void Primitives::bindShaderAttributes()
    {
        AIKO_NOT_IMPLEMENTED
    }

    namespace light
    {

        LightData directionalLight()
        {
            LightData light = {};
            light.type = LightType::Dir;
            light.intensity = 1.0f;
            light.color = { 1.0f };
            light.direction = { -0.2f, -1.0f, -0.3f };
            light.ambient = { 0.05f, 0.05f, 0.05f };
            light.diffuse = { 0.4f, 0.4f, 0.4f };
            light.specular = { 0.5f, 0.5f, 0.5f };
            return light;
        };

        LightData pointLight()
        {
            LightData light = {};
            light.type = LightType::Point;
            light.intensity = 1.0f;
            light.color = { 1.0f };
            light.position = { 0.0f, 0.0f, 0.0f };
            light.ambient = { 0.05f, 0.05f, 0.05f };
            light.diffuse = { 0.4f, 0.4f, 0.4f };
            light.specular = { 0.5f, 0.5f, 0.5f };
            light.constant = 1.0f;
            light.linear = 0.09f;
            light.quadratic = 0.032f;
            return light;
        };

        LightData spotLight()
        {
            LightData light = {};
            light.type = LightType::Spot;
            light.intensity = 1.0f;
            light.color = { 1.0f };
            light.position = { 0.0f, 0.0f, 0.0f };
            light.direction = { -0.2f, 1.0f, -0.3f };
            light.ambient = { 0.0f, 0.0f, 0.0f };
            light.diffuse = { 1.0f, 1.0f, 1.0f };
            light.specular = { 1.0f, 1.0f, 1.0f };
            light.constant = 1.0f;
            light.linear = 0.09f;
            light.quadratic = 0.032f;
            light.cutOff = math::cos(math::radians(12.5f));
            light.outerCutOff = math::cos(math::radians(15.0f));
            return light;
        };

    }

    void Primitives::setUniforms(vec4 color)
    {
        AIKO_NOT_IMPLEMENTED
    }

    // 2D

    void Primitives::drawPoint(vec3 pos, vec4 color)
    {
        AIKO_NOT_IMPLEMENTED
    }

    void Primitives::drawTriangle(vec3 pos, float radius, vec4 color)
    {
        AIKO_NOT_IMPLEMENTED
    }

    void Primitives::drawTriangle(vec3 a, vec3 b, vec3 c, vec4 color)
    {
        AIKO_NOT_IMPLEMENTED
    }

    void Primitives::drawRectangle(vec3 pos, vec3 size, vec4 color)
    {
        AIKO_NOT_IMPLEMENTED
    }

    void Primitives::renderLine(vec3 start, vec3 end, vec4 color)
    {
        AIKO_NOT_IMPLEMENTED
    }

    void Primitives::renderCircle(vec3 pos, float radius, int numSegments, vec4 color)
    {
        AIKO_NOT_IMPLEMENTED
    }

    void Primitives::renderNgon(vec3 pos, float size, unsigned int ngon, vec4 color)
    {
        AIKO_NOT_IMPLEMENTED
    }

    // 3D

    void Primitives::drawPyramid(vec3 pos, float baseWidth, float height, vec4 color)
    {
        AIKO_NOT_IMPLEMENTED
    }

    void Primitives::drawCube(vec3 pos, vec3 size, vec4 color)
    {
        AIKO_NOT_IMPLEMENTED
    }

    void Primitives::renderSphere(vec3 pos, float radius, int numSegments, vec4 color)
    {
        AIKO_NOT_IMPLEMENTED
    }

    void Primitives::renderPolygon(vec3 pos, float radius, int rings, int sectors, vec4 color)
    {
        AIKO_NOT_IMPLEMENTED
    }

    void Primitives::renderCylinder(vec3 pos, float radius, float height, int sectors, vec4 color)
    {
        AIKO_NOT_IMPLEMENTED
    }

}
