#ifdef AIKO_BGFX

#pragma once

#include <stack>

#include "aiko.h"
#include "aiko_types.h"
#include "modules/render/render_module.h"
#include "types/textures.h"
#include "models/shader.h"
#include "models/texture.h"
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

namespace aiko::bgfx
{

    class BgfxRenderModule : public aiko::RenderModule
    {
    
    public:

        using ViewId = uint16_t;

        BgfxRenderModule(Aiko* aiko);
        virtual ~BgfxRenderModule();

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
        void renderMesh(Camera*, Transform*, Mesh*, Shader*, Texture*);
        void renderMesh(Camera*, Transform*, Mesh*, Shader*, texture::PboTexture*);

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
        texture::Texture createTexture() override;
        virtual texture::Texture createTexture(int width, int height) override;
        virtual texture::Texture loadTexture(const char*) override;
        texture::PboTexture createPboTexture(uint16_t, uint16_t);
        void updatePboTexture(texture::PboTexture texture, std::vector<Color>&);
        void drawTextureEx(texture::Texture texture, vec2 position, float rotation, float scale, Color tint);
        void drawRenderTextureEx(texture::RenderTexture2D texture, vec2 position, float rotation, float scale, Color tint);

        // Shader
        virtual aiko::ShaderData loadShaderData(const char*, const char*) override;
        virtual aiko::ShaderData loadShaderSrc(const char*, const char*) override;
        virtual void unloadShader(aiko::ShaderData& ) override;
        virtual int loadShaderUniform(Shader* shader, const string& name, ShaderUniformDataType type) override;
        virtual void setShaderUniform(Shader*, string name, vec4 value) override;
        void refreshShader(Shader*);

    protected:

        virtual void onWindowResize(Event&) override;
    
    private:

        const ViewId m_kViewOffScreen;
        const ViewId m_kViewMain;

        ViewId currentViewId;

    };

}

#endif