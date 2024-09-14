#pragma once

#include <map>

#include "aiko_types.h"
#include "modules/base_module.h"
#include "types/textures.h"
#include "models/shader.h"
#include "events/event.hpp"
#include "types/color.h"
#include "types/textures.h"

#include "modules/render/renderer_context.h"

namespace aiko
{

    class Camera;
    class DisplayModule;
    class Mesh;
    class Shader;
    class Transform;

    class RenderModule : public BaseModule
    {
    
    public:

        using RenderersCtx = std::map<ContextType, AikoPtr<RenderContext>>;

        RenderModule(Aiko* aiko);
        virtual ~RenderModule() = default;

    protected:

        virtual void connect(ModuleConnector*);
        virtual void init() override;
        virtual void postInit() override;

        virtual void beginFrame() override;
        virtual void endFrame() override;
        virtual void dispose() override;

    public:

        ivec2 getDisplaySize();

        virtual void initMesh(Mesh*) = 0;
        virtual void refreshMesh(Mesh*) = 0;
        virtual void renderMesh(Camera*, Transform*, Mesh*, Shader*) = 0;
        virtual void renderMesh(Camera*, Transform*, Mesh*, Shader*, texture::Texture*) = 0;

        virtual void refreshShader(Mesh*) = 0;

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
        virtual texture::PboTexture createPboTexture(uint16_t, uint16_t) = 0;
        virtual void updatePboTexture(texture::PboTexture texture, std::vector<Color>&) = 0;
        virtual void drawTextureEx(texture::Texture texture, vec2 position, float rotation, float scale, Color tint) = 0;
        virtual void drawRenderTextureEx(texture::RenderTexture2D texture, vec2 position, float rotation, float scale, Color tint) = 0;

        // Shader
        virtual AikoPtr<Shader> createShader() = 0;
        virtual aiko::ShaderData loadShaderData(const char*, const char*) = 0;
        virtual aiko::ShaderData loadShaderSrc(const char*, const char*) = 0;
        virtual void unloadShader(aiko::ShaderData&) = 0;

        // RenderContext
        RenderContext* getRenderer(ContextType);
        template<class Ctx> Ctx* getRenderer(ContextType);

    protected:

        Color background_color;
        bool m_scale;

        DisplayModule* m_displayModule;

        virtual void onWindowResize(Event&);

        RenderersCtx& getRenderers();

    private:

        RenderersCtx m_renderers;
    
    };

    template<class Ctx>
    inline Ctx* RenderModule::getRenderer(ContextType ctx)
    {
        RenderContext* context = getRenderer(ctx);
        if (context == nullptr)
        {
            Log::error("Render Context not supported for selected platform");
            assert(false);
            return nullptr;
        }
        return dynamic_cast<Ctx*>(context);
    }

}
