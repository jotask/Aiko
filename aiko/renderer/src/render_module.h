#pragma once

#include <map>

#include <events/event.hpp>
#include <aiko_types.h>

#include "models/shader.h"
#include "models/model.h"
#include "models/texture.h"
#include "types/color.h"
#include "types/textures.h"
#include "types/render_types.h"

namespace aiko
{

    class Camera;
    class DisplayModule;
    class Mesh;
    class Shader;
    class Transform;

    class AikoRenderer
    {
    
    public:

        AikoRenderer();
        ~AikoRenderer() = default;

    protected:

        void init();

        void beginFrame();
        void endFrame();
        void dispose();

    public:

        ivec2 getDisplaySize();

        virtual void initMesh(Mesh*) = 0;
        virtual void refreshMesh(Mesh*) = 0;
        virtual void renderMesh(Camera*, Transform*, Mesh*, Shader*) = 0;
        virtual void renderMesh(Camera*, Transform*, Mesh*, Shader*, Texture*) = 0;
        virtual void renderMesh(Camera*, Transform*, Mesh*, Shader*, texture::PboTexture*) = 0;
        virtual void renderTransientBuffer(Camera*, Transform*, Shader*, Mesh*) = 0;
        virtual void renderModel(Camera*, Transform*, Model*) = 0;

        virtual texture::RenderTexture2D* getRenderTexture() = 0;

        virtual void clearBackground(Color) = 0;
        virtual void beginMode2D() = 0;
        virtual void endMode2D() = 0;
        virtual void beginMode3D() = 0;
        virtual void endMode3D() = 0;
        virtual void beginTextureMode() = 0;
        virtual void beginTextureMode(texture::RenderTexture2D& target) = 0;
        virtual void endTextureMode(void) = 0;
        virtual void beginShaderMode(aiko::Shader* shader) = 0;
        virtual void endShaderMode(void) = 0;
        virtual void beginBlendMode(BlendMode) = 0;
        virtual void endBlendMode(void) = 0;

        virtual void initScreenFbo() = 0;
        virtual texture::ScreenFbo getScreenFbo() = 0;

        // Font
        virtual void drawText(string, float, float, float = 1.0f, Color = WHITE) = 0;

        // Texture
        virtual texture::Texture createTexture() = 0;
        virtual texture::Texture createTexture(int width, int height) = 0;
        virtual texture::Texture loadTexture(const char*) = 0;
        virtual texture::PboTexture createPboTexture(uint16_t, uint16_t) = 0;
        virtual void updatePboTexture(texture::PboTexture texture, std::vector<Color>&) = 0;

        void setBackgroundColor(const Color color);

    protected:

        Color background_color;
        bool m_scale;

        texture::ScreenFbo m_screenFbo;
        Shader m_passthrought;

        DisplayModule* m_displayModule;

        virtual void onWindowResize(Event&);
    
    };

}
