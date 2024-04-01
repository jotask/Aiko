#pragma once

#include "shared/math.h"
#include "models/shader.h"

namespace aiko
{
    class Shader;
    class RenderSystem;
    class Primitives
    {
        friend class RenderSystem;
    private:

        // #define DEFAULT_2D_COLOR { 1.0f   , 0.686f , 0.8f    , 1.0f }
        #define DEFAULT_2D_COLOR { 1.0f   , 0.0f   , 0.0f    , 1.0f }
        #define DEFAULT_3D_COLOR { 1.0f   , 1.0f   , 1.0f    , 1.0f }

        static AikoPtr<Shader> shader;
        static RenderSystem* renderSystem;

        static void init(RenderSystem*);

        static void use();
        static void unuse();

        static void bindShaderAttributes();
        static void setUniforms(vec4 color);

    public:
        // 2D
        static void drawPoint(vec3, vec4 = DEFAULT_2D_COLOR);
        static void drawTriangle(vec3, float, vec4 = DEFAULT_2D_COLOR);
        static void drawTriangle(vec3, vec3, vec3, vec4 = DEFAULT_2D_COLOR);
        static void drawRectangle(vec3, vec3, vec4 = DEFAULT_2D_COLOR);
        static void renderLine(vec3, vec3, vec4 = DEFAULT_2D_COLOR);
        static void renderCircle(vec3, float, int = 50, vec4 = DEFAULT_2D_COLOR);
        static void renderNgon(vec3, float, unsigned int, vec4 = DEFAULT_2D_COLOR);

        // 3d
        static void drawPyramid(vec3, float baseWidth, float height, vec4 = DEFAULT_3D_COLOR);
        static void drawCube(vec3, vec3, vec4 = DEFAULT_3D_COLOR);
        static void renderSphere(vec3, float, int = 50, vec4 = DEFAULT_3D_COLOR);
        static void renderPolygon(vec3, float, int rings, int sectors, vec4 = DEFAULT_3D_COLOR);
        static void renderCylinder(vec3 pos, float radius, float height, int sectors, vec4 = DEFAULT_3D_COLOR);

        };

}

