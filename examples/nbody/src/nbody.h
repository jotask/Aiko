#pragma once

#include "application/application.h"

namespace nbody
{

    class NBody : public aiko::Layer
    {
    protected:

        virtual void registerSystems(aiko::SystemRegistry&) override;
        virtual void init() override;
    private:
    
    };

}

