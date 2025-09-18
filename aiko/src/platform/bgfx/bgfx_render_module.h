#ifdef AIKO_BGFX

#pragma once

#include <stack>

#include "aiko.h"
#include "aiko_types.h"
#include "modules/render/render_module.h"
#include "types/textures.h"
#include "models/shader.h"
#include "models/model.h"
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

        virtual void initMesh(Mesh*) override;
        virtual void refreshMesh(Mesh*) override;
        virtual void renderMesh(Camera*, Transform*, Mesh*, Shader*) override;
        virtual void renderMesh(Camera*, Transform*, Mesh*, Shader*, Texture*) override;
        virtual void renderMesh(Camera*, Transform*, Mesh*, Shader*, texture::PboTexture*) override;
        virtual void renderTransientBuffer(Camera*, Transform*, Shader*, Mesh*) override;
        virtual void renderModel(Camera*, Transform*, Model*) override;

        virtual texture::RenderTexture2D* getRenderTexture() override;

        virtual void clearBackground(Color) override;
        virtual void beginMode2D() override;
        virtual void endMode2D() override;
        virtual void beginMode3D() override;
        virtual void endMode3D() override;
        virtual void beginTextureMode() override;
        virtual void beginTextureMode(texture::RenderTexture2D& target) override;
        virtual void endTextureMode(void) override;
        virtual void beginShaderMode(aiko::Shader* shader) override;
        virtual void endShaderMode(void) override;
        virtual void beginBlendMode(BlendMode) override;
        virtual void endBlendMode(void) override;

        virtual void initScreenFbo() override;
        virtual texture::ScreenFbo getScreenFbo() override;

        // Font
        virtual void drawText(string, float, float, float = 1.0f, Color = WHITE) override;

        // Texture
        virtual texture::Texture createTexture() override;
        virtual texture::Texture createTexture(int width, int height) override;
        virtual texture::Texture loadTexture(const char*) override;
        virtual texture::PboTexture createPboTexture(uint16_t, uint16_t) override;
        virtual void updatePboTexture(texture::PboTexture texture, std::vector<Color>&) override;

        // Shader
        virtual aiko::ShaderData loadShaderData(const char*, const char*) override;
        virtual aiko::ShaderData loadShaderSrc(const char*, const char*) override;
        virtual void unloadShader(aiko::ShaderData& ) override;
        virtual int loadShaderUniform(Shader* shader, const string& name, ShaderUniformDataType type) override;
        virtual void setShaderUniform(Shader*, string name, vec4 value) override;
        virtual void refreshShader(Shader*) override;

    protected:

        virtual void onWindowResize(Event&) override;
    
    private:

        const ViewId m_kViewOffScreen;
        const ViewId m_kViewMain;

        ViewId currentViewId;

    };

}

#endif