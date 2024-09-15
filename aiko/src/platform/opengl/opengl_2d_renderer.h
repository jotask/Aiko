#pragma once

#include <vector>

#include "modules/render/2d_renderer.h"
#include "modules/render/render_module.h"
#include "models/shader.h"

namespace aiko
{
    class RenderModule;
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

            std::vector<vec2> spritePositions;
            std::vector<Color> spriteColors;
            std::vector<vec2> texturePositions;
            std::vector<texture::Texture> spriteTextures;
            uint buffers[Renderer2DBufferType::BufferSize] = {};

            AikoPtr<Shader> shader;
            texture::Texture defaultTexture;
        };

        Screen root;
        Vertex objs;

        void drawAbsRotation(Camera* cam, vec2 pos, vec2 size, const texture::Texture texture, const Color color, const vec2 origin = {}, const float rotationDegrees = 0.f, const vec4 textureCoords = GL2D_DefaultTextureCoords);

        vec2 positionToScreenCoords(vec2, vec2);
        vec2 rotateAroundPoint(vec2, vec2, float);
        vec2 scaleAroundPoint(vec2, vec2, float);

    };
}
