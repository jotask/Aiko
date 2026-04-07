#pragma once

#include "application/application.h"
#include "components/light_component.h"
#include "world/world.h"

namespace aiko
{
    class LightComponent;
}

namespace vw
{

    class VoxelWorld : public aiko::Layer
    {
    public:
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;
    private:

        World m_world;

        aiko::AikoPtr<aiko::LightComponent> m_lightComponent;

    };

}

