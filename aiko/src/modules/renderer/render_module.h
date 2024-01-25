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

    virtual void preRender() override;
    virtual void render() override;
    virtual void postRender() override;

    virtual void beginFrame() override;
    virtual void endFrame() override;

private:

    RenderType m_currentRenderType;
    aiko::AikoUPtr<RendererComponent> m_renderType;

};