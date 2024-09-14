#ifdef AIKO_NATIVE

#pragma once

#include "aiko.h"
#include "aiko_types.h"
#include "modules/render/render_module.h"
#include "types/textures.h"
#include "models/shader.h"
#include "events/event.hpp"
#include "types/color.h"
#include "types/textures.h"

namespace aiko
{

    class Camera;
    class DisplayModule;
    class Mesh;
    class Shader;
    class Transform;
}

namespace aiko::native
{

    class OpenglRenderModule : public aiko::RenderModule
    {
    
    public:

        OpenglRenderModule(Aiko* aiko);
        virtual ~OpenglRenderModule();

    protected:
    
        virtual void preInit() override;
        virtual void init() override;

        virtual void beginFrame() override;
        virtual void endFrame() override;

        virtual void dispose() override;
    
    public:

        void initMesh(Mesh*);
        void refreshMesh(Mesh*);
        void renderMesh(Camera*, Transform*, Mesh*, Shader*);
        void renderMesh(Camera*, Transform*, Mesh*, Shader*, texture::Texture*);

        void refreshShader(Mesh*);

        texture::RenderTexture2D* getRenderTexture();

        void clearBackground(Color);
        void beginMode2D();
        void endMode2D();
        void beginMode3D();
        void endMode3D();
        void beginTextureMode();
        void beginTextureMode(texture::RenderTexture2D& target);
        void endTextureMode(void);
        void beginShaderMode(aiko::Shader* shader);
        void endShaderMode(void);
        void beginBlendMode(BlendMode);
        void endBlendMode(void);

        void initScreenFbo();
        texture::ScreenFbo getScreenFbo();

        // Font
        void drawText(string, float, float, float = 1.0f, Color = WHITE);

        // Texture
        texture::Texture createTexture();
        texture::PboTexture createPboTexture(uint16_t, uint16_t);
        void updatePboTexture(texture::PboTexture texture, std::vector<Color>&);
        void drawTextureEx(texture::Texture texture, vec2 position, float rotation, float scale, Color tint);
        void drawRenderTextureEx(texture::RenderTexture2D texture, vec2 position, float rotation, float scale, Color tint);

        // Shader
        AikoPtr<Shader> createShader();
        aiko::ShaderData loadShaderData(const char*, const char*);
        aiko::ShaderData loadShaderSrc(const char*, const char*);
        void unloadShader(aiko::ShaderData& );

    protected:

        ScreenFbo m_screenFbo;
        AikoPtr<Shader> m_passthrought;

        virtual void onWindowResize(Event&) override;
    
    };

}

#endif