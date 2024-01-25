#pragma once

#include "aiko_types.h"
#include "modules/base_module.h"
#include "modules/scene_module.h"

class RendererComponent
{

public:

    virtual void preInit() { };
    virtual void init() { };
    virtual void postInit() { };

    virtual void preUpdate() { };
    virtual void update() { };
    virtual void postUpdate()  { };

    virtual void preRender()  { };
    virtual void render()  { };
    virtual void postRender()  { };

    virtual void beginFrame() { };
    virtual void endFrame() { };

    virtual void dispose() { };

protected:

};