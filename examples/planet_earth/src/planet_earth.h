#pragma once

#include "aiko_includes.h"
#include "layers/layer.h"

namespace pe
{

    class PlanetEarth : public aiko::Layer
    {
    protected:
        virtual void connect(aiko::SystemConnector&) override;
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;
    private:

    };

}

