#pragma once

#include <vector>

#include "shared/math.h"
#include "types/textures.h"
#include "types/color.h"
#include "modules/render/2d_renderer.h"
#include "modules/render/render_module.h"

namespace aiko
{
    class RenderModule;
    class Camera;
    class Opengl2DRenderer : public RenderContext2D
    {
    public:

        enum Renderer2DBufferType
        {
            QuadPositions,
            QuadColors,
            TexturePositions,
            BufferSize
        };

        Opengl2DRenderer(RenderModule* renderer);
        virtual ~Opengl2DRenderer() = default;

        virtual void init();
        virtual void beginFrame();
        virtual void endFrame();
        virtual void dispose();

        virtual void drawRectangle(Camera* cam, vec2 pos, vec2 size, Color color);

    private:

        #define GL2D_DefaultTextureCoords (vec4{ 0, 1, 1, 0 })

        virtual texture::ScreenFbo getFbo();

        void drawAbsRotation(Camera* cam, vec2 pos, vec2 size, const texture::Texture texture, const Color color, const vec2 origin = {}, const float rotationDegrees = 0.f, const vec4 textureCoords = GL2D_DefaultTextureCoords);

        const unsigned int BATCH_SIZE = 1'000;

        void clearBatch();
#
        void enableNecessaryGLFeatures();
        vec2 positionToScreenCoords(vec2, vec2);
        vec2 rotateAroundPoint(vec2, vec2, float);
        vec2 scaleAroundPoint(vec2, vec2, float);

        std::vector<vec2> m_spritePositions;
        std::vector<vec4> m_spriteColors;
        std::vector<vec2> m_texturePositions;
        std::vector<texture::Texture> m_spriteTextures;
        uint buffers[Renderer2DBufferType::BufferSize] = {};

        AikoPtr<Shader> m_defaultShader;
        texture::Texture m_white1pxSquareTexture;

        RenderModule::ScreenFbo m_fbo;


    };
}
