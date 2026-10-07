#pragma once

#include <layers/layer.h>

namespace nbody
{

    class NBody : public aiko::Layer
    {
    protected:
        virtual void registerSystems(aiko::SystemRegistry&) override;
        virtual void init() override;

    };

}

