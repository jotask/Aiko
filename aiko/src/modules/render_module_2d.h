#pragma once

#include "aiko_types.h"
#include "base_module.h"
#include "modules/scene_module.h"

class RenderModule2D : public BaseModule
{

public:

    RenderModule2D() = default;
    virtual ~RenderModule2D() = default;

    virtual void connect(ModuleConnector*) override;

    virtual void preInit() override;
    virtual void init() override;
    virtual void postInit() override;

    void virtual preUpdate() override;
    void virtual update() override;
    void virtual postUpdate() override;

    void virtual preRender() override;
    void virtual render() override;
    void virtual postRender() override;

protected:

    aiko::AikoPtr<SceneModule> m_sceneModule;

};