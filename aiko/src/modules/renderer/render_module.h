#pragma once

#include "aiko_types.h"
#include "modules/base_module.h"
#include "modules/renderer/renderer_component.h"
#include "core/textures.h"
#include "models/shader.h"
#include "events/event.hpp"

namespace aiko
{

    class CameraModule;
    class DisplayModule;
    class Shader;
    
    class RenderModule : public BaseModule
    {
    
    public:
    
        enum class RenderType
        {
            TwoDimensions,
            ThreeDimensions,
            Texture,
            Pixel,
        };
    
        RenderModule();
        virtual ~RenderModule();

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
    
        CameraModule* getCameraModule() { return m_cameraModule; }
        RendererComponent* GetRenderComponent() { return m_renderType.get(); };
        ivec2 getDisplaySize();

        texture::RenderTexture2D* getRenderTexture();
    
    private:

        CameraModule* m_cameraModule;
        DisplayModule* m_displayModule;
        RenderType m_currentRenderType;
        aiko::AikoUPtr<RendererComponent> m_renderType;

        texture::RenderTexture2D m_renderTexture2D;

        bool m_isImguiDemoOpen;

        void updateRenderType(RenderModule::RenderType newRenderType, bool autoInit = true);

        void onWindowResize(Event&);

        void clearBackground();
        void beginMode2D();
        void endMode2D();
        void beginMode3D();
        void endMode3D();
        void beginTextureMode(texture::RenderTexture2D* target);
        void endTextureMode(void);
        void beginShaderMode(Shader* shader);
        void endShaderMode(void);
        void beginBlendMode(int mode);
        void endBlendMode(void);
    
    };

}
