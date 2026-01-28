#pragma once

#include "application/application.h"

namespace nbody
{

    class NBody : public aiko::Application
    {
    protected:
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;
    private:
    
    };

}

