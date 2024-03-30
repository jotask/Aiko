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

        #define DEFAULT_COLOR { 1.,0.686,0.8, 1.0f }

        static AikoPtr<Shader> shader;
        static RenderSystem* renderSystem;

        static void init(RenderSystem*);

        static void use();
        static void unuse();

        static void bindShaderAttributes();
        static void setUniforms(vec4 color);

    public:
        static void drawPoint(vec3, vec4 = DEFAULT_COLOR);
        static void drawTriangle(vec3, float, vec4 = DEFAULT_COLOR);
        static void drawTriangle(vec3, vec3, vec3, vec4 = DEFAULT_COLOR);
        static void drawRectangle(vec3, vec3, vec4 = DEFAULT_COLOR);
        static void renderLine(vec3, vec3, vec4 = DEFAULT_COLOR);
        static void renderCircle(vec3, float, int = 50, vec4 = DEFAULT_COLOR);
        static void renderNgon(vec3, float, unsigned int, vec4 = DEFAULT_COLOR);
    };

}

