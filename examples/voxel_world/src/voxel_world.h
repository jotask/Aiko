#pragma once

#include "application/application.h"
#include "components/light_component.h"
#include "world/world.h"

namespace aiko
{
    class CameraComponent;
    class PlayerControllerComponent;
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

        aiko::CameraComponent* m_playerCamera;
        aiko::PlayerControllerComponent* m_playerComponent;
        aiko::LightComponent* m_lightComponent;


    };

}

