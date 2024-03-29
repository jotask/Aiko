#pragma once

#include "shared/math.h"
#include "models/shader.h"

namespace aiko
{
    class Shader;
    class Primitives
    {
        friend class RenderSystem;
    private:
        static AikoPtr<Shader> shader;

        static void use();
        static void unuse();

        static void bindShaderAttributes();

    public:
        static void drawPoint(vec3);
        static void drawTriangle(vec3, float);
        static void drawTriangle(vec3, vec3, vec3);
        static void drawRectangle(vec3, vec3);
        static void renderLine(vec3, vec3);
        static void renderCircle(vec3, float, int = 50);
        static void renderNgon(vec3, float, unsigned int);
    };

}

