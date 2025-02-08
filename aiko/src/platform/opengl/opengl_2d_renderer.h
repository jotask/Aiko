#pragma once

#include <vector>

#include "modules/render/2d_renderer.h"
#include "modules/render/render_module.h"
#include "models/shader.h"

namespace aiko
{
    class RenderModule;
    class Camera;
    class Opengl2DRenderer : public RenderContext2D
    {
    public:

        Opengl2DRenderer(RenderModule* renderer);
        virtual ~Opengl2DRenderer() = default;

        virtual void init();
        virtual void beginFrame();
        virtual void endFrame();
        virtual void dispose();

        virtual void drawRectangle(Camera* cam, vec2 pos, vec2 size, Color color);
        virtual void drawRectangle(Camera*, vec2 pos, vec2 size, std::array<Color, 4> colors);

    private:

#define GL2D_DefaultTextureCoords (vec4{ 0, 1, 1, 0 })

        enum Renderer2DBufferType
        {
            QuadPositions,
            QuadColors,
            TexturePositions,
            BufferSize
        };

        struct Screen
        {
            uint vao;
            uint vbo;
            vec4 color;
            AikoPtr<Shader> shader;
        };

        struct Vertex
        {
            uint vao;
            uint vbo;

            std::vector<vec3> spritePositions;
            std::vector<Color> spriteColors;
            std::vector<vec2> texturePositions;
            std::vector<texture::Texture> spriteTextures;
            uint buffers[Renderer2DBufferType::BufferSize] = {};

            mat4 view = mat4(1.0f);
            mat4 projection = mat4(1.0f);

            AikoPtr<Shader> shader;
            texture::Texture defaultTexture;
        };

        Screen root;
        Vertex objs;

        void drawAbsRotation(Camera* cam, vec3 pos, vec2 size, const texture::Texture texture, const std::array<Color, 4> colors, const vec2 origin = {}, const float rotationDegrees = 0.f, const vec4 textureCoords = GL2D_DefaultTextureCoords);

        vec2 positionToScreenCoords(vec3, vec2);
        vec3 rotateAroundPoint(vec3, vec3, float);
        vec3 scaleAroundPoint(vec3, vec3, float);

    };
}
