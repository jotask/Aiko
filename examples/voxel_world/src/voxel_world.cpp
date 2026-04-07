#include "voxel_world.h"

#include <models/game_object.h>
#include <components/camera_component.h>
#include <models/camera.h>

#include <aiko_includes.h>

#include <imgui.h>

#include "voxel_world_constants.h"

namespace vw
{

    void VoxelWorld::init()
    {

        auto camera = app->Instantiate("Camera");
        auto cam = camera->addComponent<aiko::CameraComponent>(aiko::camera::CameraController::Fly);
        camera->transform().position = { 32.0f, 77.0f, 32.0f };
        cam->getCamera().position = camera->transform().position;
        cam->speed() *=  2.0f;

        m_world.setup(app->getRenderSystem(), app->m_aiko->getComponentAssetAccess());

        m_world.generate();

        auto* obj = app->Instantiate("Light");
        m_lightComponent = obj->addComponent<aiko::LightComponent>();
        m_lightComponent->setDirectional(aiko::WHITE, aiko::vec3 { 1, -1, 0}, 1.0f);

    }

    void VoxelWorld::update()
    {
        Layer::update();
        if (app->getInputSystem()->isKeyJustPressed(aiko::KEY_SPACE))
        {
            m_world.generate();
        }
        m_world.update();
    }

    void VoxelWorld::render()
    {
        Layer::render();
        m_world.render();
        m_world.gizmos();

        if (c_imgui_ambient_light)
        if (ImGui::Begin("Ambient Light"))
        {
            ImGui::SliderFloat("Intensity", &m_lightComponent->intensity, 0.0f, 1.0f);
            ImGui::SliderFloat3("Direction", &m_lightComponent->direction.x, -1.0f, 1.0f);
            ImGui::ColorEdit4("Color", &m_lightComponent->color.r);
            ImGui::End();
        }
    }
}

