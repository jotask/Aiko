#pragma once

#include "module.h"

class BaseModule : public Module
{
public:

    BaseModule() = default;
    virtual ~BaseModule() = default;

protected:

    virtual void connect(ModuleConnector*) override { };

    virtual void preInit() override { };
    virtual void init() override { };
    virtual void postInit() override { };

    virtual void preUpdate() override { };
    virtual void update() override { };
    virtual void postUpdate() override { };

    virtual void preRender() override { };
    virtual void render() override { };
    virtual void postRender() override { };

    virtual void beginFrame() override { };
    virtual void endFrame() override { };

    virtual void dispose() override { };

};