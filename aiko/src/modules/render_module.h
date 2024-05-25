#pragma once

#include "aiko_types.h"
#include "modules/base_module.h"
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

    class RenderModule : public BaseModule
    {
    
    public:

        RenderModule(Aiko* aiko);
        virtual ~RenderModule();

    protected:

        virtual void connect(ModuleConnector*);
    
        virtual void preInit() override;
        virtual void init() override;
        virtual void postInit() override;
    
        void virtual preUpdate() override;
        void virtual update() override;
        void virtual postUpdate() override;
    
        virtual void preRender() override;
        virtual void render() override;
        virtual void postRender() override;
    
        virtual void beginFrame() override;
        virtual void endFrame() override;

        virtual void dispose() override;
    
    public:

        void initMesh(Mesh*);
        void refreshMesh(Mesh*);
        void renderMesh(Camera*, Transform*, Mesh*, Shader*);
        void renderMesh(Camera*, Transform*, Mesh*, Shader*, texture::Texture*);

        void refreshShader(Mesh*);

        ivec2 getDisplaySize();

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

        // Font
        void drawText(std::string, float, float, float = 1.0f, Color = WHITE);

        // Texture
        texture::RenderTexture2D createRenderTexture();
        texture::Texture createTexture();
        texture::PboTexture createPboTexture(uint16_t, uint16_t);
        void updatePboTexture(texture::PboTexture texture, std::vector<Color>&);
        void drawTextureEx(texture::Texture texture, vec2 position, float rotation, float scale, Color tint);
        void drawRenderTextureEx(texture::RenderTexture2D texture, vec2 position, float rotation, float scale, Color tint);
        // void drawTexture(texture::Texture texture, int posX, int posY, Color tint);
        // void drawTextureV(texture::Texture texture, vec2 position, Color tint);
        // void drawTextureRec(texture::Texture texture, Rectangle source, vec2 position, Color tint);
        // void drawTexturePro(texture::Texture texture, Rectangle source, Rectangle dest, vec2 origin, float rotation, Color tint);

        // Shader
        AikoUPtr<Shader> createShader();
        aiko::ShaderData loadShaderData(const char*, const char*);
        void unloadShader( aiko::ShaderData& );
        int getShaderLocation(aiko::ShaderData&, const char* uniformName );
        void setShaderUniformValue(aiko::ShaderData&, int, const void*, aiko::ShaderUniformDataType);
        void setShaderUniformValueV(aiko::ShaderData&, int, const void*, aiko::ShaderUniformDataType, int);

    private:

        Color background_color;
        bool m_scale;

        DisplayModule* m_displayModule;

        texture::RenderTexture2D m_renderTexture2D;

        AikoUPtr<Shader> m_passthrought;

        void onWindowResize(Event&);
        void renderToTargetTexture();
    
    };

}
