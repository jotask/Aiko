#include "camera_system.h"

#include "components/camera_component.h"
#include "scene/scene.h"
#include "systems/input_system.h"
#include "systems/scene_system.h"
#include "systems/system_connector.h"
#include "camera/camera_controller.h"

#include <math/math.h>
#include <time/time.h>

namespace aiko
{
    void CameraSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        BIND_SYSTEM_REQUIRED(InputSystem, systemConnector, m_inputSystem);
        BIND_SYSTEM_REQUIRED(SceneSystem, systemConnector, m_sceneSystem);
    }

    void CameraSystem::update()
    {
        BaseSystem::update();

        Scene& scene = m_sceneSystem->getScene();

        for (CameraComponent* component : scene.components<CameraComponent>())
        {
            if (component == nullptr || component->isActiveAndEnabled() == false)
            {
                continue;
            }
            updateCamera(*component);
        }
    }

    void CameraSystem::updateCamera(CameraComponent& component)
    {
        Camera& camera = component.getCamera();

        switch (component.getCameraController())
        {
        case camera::CameraController::Orbit:
        {

            const float dt = Time::it().getDeltaTime();
            const float radius = component.radius();

            if (radius <= 0.0f)
            {
                break;
            }

            component.orbitAngle() += (component.speed() / radius) * dt;

            const float angle = component.orbitAngle();

            camera.position = {
                std::sin(angle) * radius,
                camera.position.y,
                std::cos(angle) * radius
            };
        }
        break;

        case camera::CameraController::Fly:
        {
            const auto dt = Time::it().getDeltaTime();

            vec3 forward = math::normalize(camera.target - camera.position);

            vec3 right = math::normalize(math::cross(forward, camera.getUp()));

            if (m_inputSystem->isKeyJustPressed(Key::KEY_F1))
            {
                m_inputSystem->setMouseCaptured(!m_inputSystem->isMouseCaptured());
            }

            if (m_inputSystem->isMouseCaptured() == true)
            {
                const vec2 mouseDelta = m_inputSystem->getMouseDelta();

                constexpr float sensitivityDegrees = 0.06f;
                const float yawDegrees = mouseDelta.x * sensitivityDegrees;
                const float pitchDegrees = mouseDelta.y * sensitivityDegrees;

                forward = math::rotate(forward, math::radians(-pitchDegrees), right);
                forward = math::rotate(forward, math::radians(-yawDegrees), camera.getUp());
                forward = math::normalize(forward);

                camera.target = camera.position + forward;
            }

            vec3 moveDir = vec3(0.0f);

            if (m_inputSystem->isKeyPressed(Key::KEY_W))
            {
                moveDir += forward;
            }

            if (m_inputSystem->isKeyPressed(Key::KEY_S))
            {
                moveDir -= forward;
            }

            if (m_inputSystem->isKeyPressed(Key::KEY_A))
            {
                moveDir -= right;
            }

            if (m_inputSystem->isKeyPressed(Key::KEY_D))
            {
                moveDir += right;
            }

            float speed = component.speed();

            if (m_inputSystem->isKeyPressed(Key::KEY_LEFT_SHIFT))
            {
                speed *= 2.0f;
            }

            camera.position += moveDir * (speed * dt);
            camera.target += moveDir * (speed * dt);
        }
        break;

        case camera::CameraController::Drag:
        {
            camera::updateDrag(camera, *m_inputSystem);
        }
        break;

        case camera::CameraController::Static:
            break;

        default:
            logger::Log::error("CAMERA :: UPDATE :: UNKNOW CONTROLLER");
            break;
        }
    }
}
