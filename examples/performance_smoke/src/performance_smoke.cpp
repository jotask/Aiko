#include "performance_smoke.h"

#include "components/camera_component.h"
#include "components/light_component.h"
#include "components/mesh_component.h"
#include "layers/contexts/scene_context.h"
#include "models/game_object.h"
#include "models/mesh_factory.h"
#include "time/time.h"
#include "types/color.h"

#include <cmath>
#include <iostream>

namespace aiko::perf
{
    namespace
    {
        constexpr int GridWidth = 100;
        constexpr int GridDepth = 100;

        constexpr float ObjectSpacing = 2.5f;

        constexpr int AnimatedObjectStride = 10;
        constexpr float AnimationSpeed = 25.0f;

        constexpr size_t ObjectCount = static_cast<size_t>(GridWidth) * static_cast<size_t>(GridDepth);

    }

    void PerformanceSmoke::init()
    {
        scene().clearColor() = RAYWHITE;

        scene().ambientLight().color = WHITE;
        scene().ambientLight().intensity = 0.1f;

        initCamera();
        initWorld();
        initLights();

        std::cout
            << "[PerformanceSmoke] initialized "
            << ObjectCount
            << " mesh objects, "
            << m_animatedObjects.size()
            << " animated"
            << '\n';
    }

    void PerformanceSmoke::update()
    {
        updateAnimatedObjects();
        updateStats();
    }

    void PerformanceSmoke::initCamera()
    {
        GameObject* camera = Instantiate("Camera");

        CameraComponent* cameraComponent = camera->addComponent<CameraComponent>(camera::CameraController::Fly);

        camera->transform().position =
        {
            0.0f,
            18.0f,
            35.0f
        };

        cameraComponent->getCamera().position = camera->transform().position;
    }

    void PerformanceSmoke::initWorld()
    {
        GameObject* root = Instantiate("World");

        const MeshAsset cube = mesh::factory::generateCube();

        const float halfWidth = static_cast<float>(GridWidth - 1) * ObjectSpacing * 0.5f;

        const float halfDepth = static_cast<float>(GridDepth - 1) * ObjectSpacing * 0.5f;

        int objectIndex = 0;

        for (int z = 0; z < GridDepth; ++z)
        {
            for (int x = 0; x < GridWidth; ++x)
            {
                GameObject* object = Instantiate(root, "Cube");

                object->transform().position =
                {
                    static_cast<float>(x) * ObjectSpacing - halfWidth,
                    0.0f,
                    -static_cast<float>(z) * ObjectSpacing + halfDepth
                };

                MeshComponent* meshComponent = object->addComponent<MeshComponent>();

                meshComponent->load(cube);

                Material& material = meshComponent->getMaterial();

                material.m_lit = true;

                switch (objectIndex % 4)
                {
                    case 0:
                        material.m_baseColor = RED;
                        break;

                    case 1:
                        material.m_baseColor = GREEN;
                        break;

                    case 2:
                        material.m_baseColor = BLUE;
                        break;

                    default:
                        material.m_baseColor = YELLOW;
                        break;
                }

                if ((objectIndex % AnimatedObjectStride) == 0)
                {
                    m_animatedObjects.push_back(object);
                }

                ++objectIndex;
            }
        }
    }

    void PerformanceSmoke::initLights()
    {
        constexpr float LightRange = 40.0f;
        constexpr float LightIntensity = 1.0f;

        const vec3 positions[] =
        {
            {-30.0f, 15.0f,  20.0f},
            { 30.0f, 15.0f,  20.0f},
            {-30.0f, 15.0f, -20.0f},
            { 30.0f, 15.0f, -20.0f}
        };

        const Color colors[] =
        {
            RED,
            GREEN,
            BLUE,
            WHITE
        };

        GameObject* root = Instantiate("Lights");

        for (size_t i = 0; i < 4; ++i)
        {
            GameObject* object = Instantiate(root, "PointLight");

            object->transform().position = positions[i];

            LightComponent* light = object->addComponent<LightComponent>();

            light->setPointLight(colors[i], LightRange);

            light->intensity = LightIntensity;
        }
    }

    void PerformanceSmoke::updateAnimatedObjects()
    {
        static float angle = 0.0f;

        angle += AnimationSpeed * getDeltaTime();

        angle = std::fmod(angle, 360.0f);

        for (size_t i = 0; i < m_animatedObjects.size(); ++i)
        {
            GameObject* object = m_animatedObjects[i];

            object->transform().rotation =
            {
                angle + static_cast<float>(i),
                angle * 0.5f,
                0.0f
            };
        }
    }

    void PerformanceSmoke::updateStats()
    {
        m_statsTimer += getDeltaTime();

        if (m_statsTimer < 1.0f)
        {
            return;
        }

        m_statsTimer = 0.0f;

        const float fps = Time::it().getAverageFps();

        const float frameMs = fps > 0.0f ? 1000.0f / fps : 0.0f;

        std::cout
            << "[PerformanceSmoke] "
            << "objects=" << ObjectCount
            << " animated=" << m_animatedObjects.size()
            << " fps=" << fps
            << " frame=" << frameMs << " ms"
            << '\n';
    }
}
