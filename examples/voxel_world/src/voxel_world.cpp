#include "voxel_world.h"

#include <models/game_object.h>
#include <components/camera_component.h>
#include <models/camera.h>

#include <aiko_includes.h>

#include "world/types/chunk_types.h"
#include "world/types/world_types.h"

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

        constexpr float radiusSpawn = WORLD_SIZE * CHUNK_SIZE.x * VOXEL_SIZE;
        for (size_t i = 0 ; i < 10; ++i )
        {
            auto* obj = app->Instantiate("Light");
            obj->transform().position = {
                aiko::utils::getRandomValue(0.0f, radiusSpawn),
                aiko::utils::getRandomValue(0.0f, radiusSpawn),
                aiko::utils::getRandomValue(0.0f, radiusSpawn),
            };
            auto cmp = obj->addComponent<aiko::LightComponent>();
            cmp->setPointLight(aiko::WHITE, 1.0f);
            const LightInst ints = {
                .obj = obj,
                .cmp = cmp,
                .angle = aiko::utils::getRandomValue(0.0f, 360.f)
            };
            m_lights.push_back(ints);
        }

    }

    void VoxelWorld::update()
    {
        Layer::update();
        if (app->getInputSystem()->isKeyJustPressed(aiko::KEY_SPACE))
        {
            m_world.generate();
        }
        m_world.update();

        {
            for (auto& light : m_lights)
            {
                light.angle += 1.0f * app->getlDeltaTime();
                aiko::vec3 pos = {std::sin(light.angle), 0.0f, std::cos(light.angle)};
                light.obj->transform().position = pos;
            }
        }

    }

    void VoxelWorld::render()
    {
        Layer::render();
        m_world.render();
    }
}

