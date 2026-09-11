#include "camera_system.h"

#include "components/camera_component.h"
#include "scene/scene.h"
#include "systems/input_system.h"
#include "systems/scene_system.h"
#include "systems/system_connector.h"

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
            if (component != nullptr)
            {
                updateCamera(*component);
            }
        }
    }

    void CameraSystem::updateCamera(CameraComponent& component)
    {
        Camera& camera = component.getCamera();

        switch (component.getCameraController())
        {
        case camera::CameraController::Orbit:
        {
            const auto timer = Time::it().secondSinceStart();

            const float camX =
                static_cast<float>(sin(timer) * component.radius());

            const float camZ =
                static_cast<float>(cos(timer) * component.radius());

            camera.position = {
                camX,
                camera.position.y,
                camZ
            };
        }
        break;

        case camera::CameraController::Fly:
        {
            const auto dt = Time::it().getDeltaTime();

            vec3 forward =
                math::normalize(camera.target - camera.position);

            vec3 right =
                math::normalize(math::cross(forward, camera.getUp()));

            if (m_inputSystem->isKeyJustPressed(Key::KEY_F1))
            {
                m_inputSystem->setIsMouseCentred(
                    !m_inputSystem->getIsMouseCentred()
                );
            }

            if (m_inputSystem->getIsMouseCentred() == true)
            {
                const vec2 mouseDelta =
                    m_inputSystem->getMouseDelta();

                const float sensitivity = 3.5f;
                const float yaw = mouseDelta.x * sensitivity;
                const float pitch = mouseDelta.y * sensitivity;

                forward = math::rotate(
                    forward,
                    math::radians(-pitch),
                    right
                );

                forward = math::rotate(
                    forward,
                    math::radians(-yaw),
                    camera.getUp()
                );

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
            if (m_inputSystem->isMouseButtonPressed(
                    MouseButton::MOUSE_BUTTON_RIGHT))
            {
                const vec2 mouseDelta =
                    m_inputSystem->getMouseDelta();

                const float sensitivity = 0.002f;

                const vec3 direction =
                    camera.position - camera.target;

                const float angleX =
                    mouseDelta.x * sensitivity;

                const float angleY =
                    mouseDelta.y * sensitivity;

                const float cosAngleX = std::cos(angleX);
                const float sinAngleX = std::sin(angleX);

                const vec3 newDirX(
                    cosAngleX * direction.x -
                        sinAngleX * direction.z,
                    direction.y,
                    sinAngleX * direction.x +
                        cosAngleX * direction.z
                );

                const vec3 right = math::normalize(
                    math::cross(direction, camera.getUp())
                );

                const float cosAngleY = std::cos(angleY);
                const float sinAngleY = std::sin(angleY);

                const vec3 newDirY = math::normalize(
                    cosAngleY * newDirX +
                    sinAngleY * camera.getUp()
                );

                camera.position =
                    camera.target +
                    newDirY * math::length(direction);
            }

            if (m_inputSystem->isMouseButtonPressed(
                    MouseButton::MOUSE_BUTTON_MIDDLE))
            {
                constexpr float panSpeed = 0.01f;

                const vec2 mouseDelta =
                    m_inputSystem->getMouseDelta();

                const vec3 right = math::normalize(
                    math::cross(
                        camera.getCameraDirection(),
                        camera.getUp()
                    )
                );

                const vec3 upMove =
                    camera.getUp() *
                    (mouseDelta.y * panSpeed);

                const vec3 rightMove =
                    right *
                    (mouseDelta.x * panSpeed);

                camera.position += rightMove + upMove;
                camera.target += rightMove + upMove;
            }

            constexpr float epsilon = 1e-6f;

            if (fabs(m_inputSystem->getMouseScrollBack().y) >
                epsilon)
            {
                constexpr float zoomSpeed = 0.5f;

                const vec3 direction = math::normalize(
                    camera.target - camera.position
                );

                const float amount =
                    m_inputSystem->getMouseScrollBack().y *
                    zoomSpeed;

                camera.position += direction * amount;
            }
        }
        break;

        case camera::CameraController::Static:
            break;

        default:
            logger::Log::error(
                "CAMERA :: UPDATE :: UNKNOW CONTROLLER"
            );
            break;
        }
    }
}
