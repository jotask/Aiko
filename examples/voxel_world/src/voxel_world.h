#pragma once

#include "application/application.h"
#include "components/light_component.h"
#include "world/world.h"

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

        struct LightInst
        {
            aiko::GameObject* obj;
            aiko::AikoPtr<aiko::LightComponent> cmp;
            float angle;
        };
        std::vector<LightInst> m_lights;

    
    };

}

