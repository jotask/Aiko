#pragma once

#include "aiko_types.h"
#include "modules/base_module.h"
#include "modules/renderer/renderer_component.h"

class RenderModule : public BaseModule
{

public:

    enum class RenderType
    {
        TwoDimensions,
        ThreeDimensions,
        Texture
    };

    RenderModule();
    virtual ~RenderModule() = default;

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

    RendererComponent* GetRenderComponent() {
        return m_renderType.get();
    };

private:

    RenderType m_currentRenderType;
    aiko::AikoUPtr<RendererComponent> m_renderType;


    bool m_isImguiDemoOpen;

};