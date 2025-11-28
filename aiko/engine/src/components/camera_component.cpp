#include "components/camera_component.h"

#include "systems/camera_system.h"
#include "systems/input_system.h"
#include "models/camera.h"
#include "models/time.h"
#include "shared/math.h"

namespace aiko
{

    CameraComponent::CameraComponent()
        : CameraComponent(camera::CameraController::Static)
    {
    }

    CameraComponent::CameraComponent(camera::CameraController controller, camera::CameraType type)
        : Component("Camera")
        , m_camera(nullptr)
        , cameraControler(controller)
        , m_type(type)
    {

    }
    
    camera::CameraType CameraComponent::getCameraType() const
    {
        return m_camera->getCameraType();
    }
    
    void CameraComponent::setCameraType(camera::CameraType newType)
    {
        m_camera->setCameraType(newType);
    }
    
    camera::CameraController CameraComponent::getCameraController() const
    {
        return cameraControler;
    }
    
    void CameraComponent::setCameraController(camera::CameraController newController)
    {
        cameraControler = newController;
    }

    void CameraComponent::update()
    {
        switch (cameraControler)
        {
        case camera::CameraController::Orbit:
        {
            auto timer = aiko::Time::it().secondSinceStart();
            float camX = static_cast<float>(sin(timer) * m_radius);
            float camZ = static_cast<float>(cos(timer) * m_radius);
            m_camera->position = { camX, m_camera->position.y, camZ };
        }
        break;
        case camera::CameraController::Fly:
        {
            const auto dt = Time::it().getDeltaTime();

            vec3 forward = math::normalize(m_camera->target - m_camera->position);
            vec3 right = math::normalize(math::cross(forward, m_camera->getUp()));

            // Mouse
            {

                static bool centred = false;

                if (m_inputSystem->isKeyJustPressed(Key::KEY_F1))
                {
                    m_inputSystem->setIsMouseCentred( !m_inputSystem->getIsMouseCentred() );
                }

                if (m_inputSystem->getIsMouseCentred() == true)
                {

                    // Get mouse movement delta
                    vec2 mouseDelta = m_inputSystem->getMouseDelta(); // Assuming this returns the change in mouse position

                    // Sensitivity settings for mouse movement
                    float sensitivity = 3.5f;
                    float yaw = mouseDelta.x * sensitivity;
                    float pitch = mouseDelta.y * sensitivity;

                    // Apply pitch (looking up/down)
                    forward = math::rotate(forward, math::radians(-pitch), right);

                    // Apply yaw (looking left/right)
                    forward = math::rotate(forward, math::radians(-yaw), m_camera->getUp());

                    // Update the target based on the new forward vector
                    m_camera->target = m_camera->position + forward;
                }

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

            float speed = m_speed;


            if (m_inputSystem->isKeyPressed(Key::KEY_LEFT_SHIFT))
            {
                speed *= 2.0f;
            }

            m_camera->position += moveDir * (speed * dt);
            m_camera->target += moveDir * (speed * dt);

        }
        break;
        case camera::CameraController::Drag:
        {
            const auto dt = Time::it().getDeltaTime();

            if (m_inputSystem->isMouseButtonPressed(MouseButton::MOUSE_BUTTON_RIGHT))
            {

                vec2 currentMousePos = m_inputSystem->getMousePosition();

                vec2 mouseDelta = m_inputSystem->getMouseDelta();

                float sensitivity = 0.002f;

                // Rotate camera based on mouse movement
                vec3 direction = m_camera->position - m_camera->target;

                float angleX = mouseDelta.x * sensitivity;
                float angleY = mouseDelta.y * sensitivity;

                // Rotate around Y axis (left/right)
                float cosAngleX = std::cos(angleX);
                float sinAngleX = std::sin(angleX);
                vec3 newDirX(
                    cosAngleX * direction.x - sinAngleX * direction.z,
                    direction.y,
                    sinAngleX * direction.x + cosAngleX * direction.z
                );

                // Rotate around the camera's right axis (up/down)
                vec3 right = math::normalize(math::cross(direction, m_camera->getUp()));
                float cosAngleY = std::cos(angleY);
                float sinAngleY = std::sin(angleY);
                vec3 newDirY = math::normalize(cosAngleY * newDirX + sinAngleY * m_camera->getUp() );

                m_camera->position = m_camera->target + newDirY * math::length(direction);

            }

            if (m_inputSystem->isMouseButtonPressed(MouseButton::MOUSE_BUTTON_MIDDLE))
            {
                constexpr const float panSpeed = 0.01f;

                const vec2 currentMousePos = m_inputSystem->getMousePosition();
                const vec2 mouseDelta = m_inputSystem->getMouseDelta();
                const vec3 right = math::normalize(math::cross(m_camera->getCameraDirection(), m_camera->getUp()));
                const vec3 upMove = m_camera->getUp() * ( mouseDelta.y * panSpeed );
                const vec3 rightMove = right * ( mouseDelta.x * panSpeed );

                m_camera->position += rightMove + upMove;
                m_camera->target += rightMove + upMove;
            }

            constexpr const float epsilon = 1e-6f;
            if (fabs(m_inputSystem->getMouseScrollBack().y) > epsilon)
            {
                constexpr const float zoomSpeed = 0.5f;
                const vec2 currentMousePos = m_inputSystem->getMousePosition();
                const vec3 direction = math::normalize(m_camera->target - m_camera->position);
                float amount = m_inputSystem->getMouseScrollBack().y * zoomSpeed;
                m_camera->position += direction * amount;
            }

        }
        break;
        case camera::CameraController::Static:
        {

        }
        break;
        default:
            Log::error("CAMERA :: UPDATE :: UNKNOW CONTROLLER");
            break;
        }
    }
    
    void CameraComponent::init()
    {
        auto system = gameobject->getSystem<CameraSystem>();
        m_inputSystem = gameobject->getSystem<InputSystem>();
        m_camera = system->createCamera(true);
        m_camera->setCameraType(m_type);
    }

}
