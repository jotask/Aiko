#include "voxel_world.h"

#include "systems/system_connector.h"
#include "voxel_world_constants.h"

#include <components/camera_component.h>
#include <components/player_controller_component.h>
#include <models/camera.h>
#include <models/game_object.h>
#include <systems/physics_system.h>

#include <aiko.h>
#include <aiko_includes.h>
#include <imgui.h>

namespace vw
{

    void VoxelWorld::init()
    {

        aiko::GameObject* camera = Instantiate("Camera");
        m_playerCamera = camera->addComponent<aiko::CameraComponent>(aiko::camera::CameraController::Static);
        camera->transform().position = { 32.0f, 77.0f, 32.0f };
        m_playerCamera->getCamera().position = camera->transform().position;
        m_playerCamera->speed() *=  2.0f;

        m_world.setup(context());

        if constexpr (s_generateWorld)
        {
            m_world.generate();
        }

        aiko::GameObject* obj = Instantiate("Light");
        m_lightComponent = obj->addComponent<aiko::LightComponent>();
        m_lightComponent->setDirectional(aiko::WHITE, aiko::vec3 { 1, -1, 0}, 1.0f);

        if constexpr (s_generatePlayer)
        {
            aiko::GameObject* player = Instantiate("Player");
            m_playerComponent = player->addComponent<aiko::PlayerControllerComponent>();
            aiko::PlayerControllerComponentDesc desc;
            desc.physics.position = {8.0f, 80.0f, 8.0f};
            desc.physics.radius = 0.4f;
            desc.physics.halfHeight = 0.9f;
            desc.physics.walkSpeed = 6.0f;
            desc.physics.jumpSpeed = 8.0f;
            desc.physics.maxSlopeAngleDegrees = 50.0f;
            desc.eyeHeight = 1.6f;
            desc.debugDraw = true;
            m_playerComponent->create(desc);
        }

         if constexpr (s_generateBall)
        {
            aiko::GameObject* ball = Instantiate("Ball");
            auto ballBody = ball->addComponent<aiko::RigidBodyComponent>();
            aiko::physics::BodyDesc desc = {};
            desc.motionType = aiko::physics::MotionType::Dynamic;
            desc.layer = aiko::physics::ObjectLayer::Moving;
            desc.activate = true;
            desc.transform.position = aiko::vec3(0.0f, 64.0f, 0.0f);
            desc.transform.rotation = aiko::vec3(0.0f);
            desc.transform.scale = aiko::vec3(1.0f);
            desc.shape.type = aiko::physics::ShapeType::Sphere;
            desc.shape.sphere.radius = 1.0f;
            desc.restitution = 0.85f;
            desc.debugDraw = true;
            ballBody->create(desc);
        }

    }

    void VoxelWorld::update()
    {

        if (isKeyJustPressed(aiko::KEY_R))
        {
            m_world.generate();
        }

        aiko::vec2 move = {0.0f};
        if (isKeyPressed(aiko::KEY_LEFT))
        {
            move.x = 1.0f;
        }
        else if (isKeyPressed(aiko::KEY_RIGHT))
        {
            move.x = -1.0f;
        }
        if (isKeyPressed(aiko::KEY_UP))
        {
            move.y = 1.0f;
        }
        else if (isKeyPressed(aiko::KEY_DOWN))
        {
            move.y = -1.0f;
        }

        m_playerComponent->setMoveInput(move);
        m_playerComponent->setJumpPressed(isKeyPressed(aiko::KEY_SPACE));

        m_world.update();

        const aiko::vec2 mouseDelta = getMouseDelta();
        constexpr float sensitivity = 0.001f;
        aiko::Camera& camera = m_playerCamera->getCamera();
        m_playerComponent->addLookDelta(mouseDelta.x * sensitivity, mouseDelta.y * sensitivity);
        camera.position = m_playerComponent->getCameraPosition();
        camera.target = camera.position + m_playerComponent->getCameraForward();
        setIsMouseCentred(true);

    }

    void VoxelWorld::render()
    {
        m_world.gizmos();
        if (c_imgui_ambient_light)
        {
            if (ImGui::Begin("Ambient Light"))
            {
                ImGui::SliderFloat("Intensity", &m_lightComponent->intensity, 0.0f, 1.0f);
                ImGui::SliderFloat3("Direction", &m_lightComponent->direction.x, -1.0f, 1.0f);
                ImGui::ColorEdit4("Color", &m_lightComponent->color.r);
                ImGui::End();
            }
        }
    }
}

