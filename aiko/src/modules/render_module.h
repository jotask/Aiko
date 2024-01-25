#pragma once

#include "base_module.h"

class RenderModule : public BaseModule
{

public:

    RenderModule() = default;
    virtual ~RenderModule() = default;

    virtual void preRender() override;
    virtual void render() override;
    virtual void postRender() override;

    virtual void beginFrame() override;
    virtual void endFrame() override;

protected:

};