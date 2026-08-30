#include "player_controller_component.h"

#include "models/game_object.h"

namespace aiko
{
    PlayerControllerComponent::PlayerControllerComponent()
        : Component("PlayerControllerComponent")
    {
    }

    PlayerControllerComponent::~PlayerControllerComponent() = default;

    void PlayerControllerComponent::create(const PlayerControllerComponentDesc& desc)
    {
        m_desc = desc;
        m_yawRadians = desc.physics.yawRadians;
        m_pitchRadians = desc.pitchRadians;
        m_eyeHeight = desc.eyeHeight;
        m_pendingInput = {};
        m_initialized = false;

        m_debug.enabled = desc.debugDraw;
        m_debug.built = false;

        gameobject->transform().position = desc.physics.position;

    }

    void PlayerControllerComponent::dispose()
    {
        physicsShutdown();
    }

    void PlayerControllerComponent::setMoveInput(const vec2& move)
    {
        m_pendingInput.move = move;
    }

    void PlayerControllerComponent::setJumpPressed(bool pressed)
    {
        m_pendingInput.jumpPressed = pressed;
    }

    void PlayerControllerComponent::addLookDelta(float yawDelta, float pitchDelta)
    {
        m_yawRadians -= yawDelta;
        m_pitchRadians -= pitchDelta;

        const float kPitchLimit = math::radians(89.0f);
        m_pitchRadians = std::clamp(m_pitchRadians, -kPitchLimit, kPitchLimit);
    }

    vec3 PlayerControllerComponent::getEyePosition() const
    {
        if (m_initialized == false)
        {
            return gameobject->transform().position + vec3(0.0f, m_eyeHeight, 0.0f);
        }

        return m_controller.getPosition() + vec3(0.0f, m_eyeHeight, 0.0f);
    }

    float PlayerControllerComponent::getYaw() const
    {
        return m_yawRadians;
    }

    float PlayerControllerComponent::getPitch() const
    {
        return m_pitchRadians;
    }

    const Transform& PlayerControllerComponent::getWorldTransform() const
    {
        return gameobject->transform();
    }

    void PlayerControllerComponent::ensurePhysicsInitialized(physics::AikoPhysics& world)
    {
        if (m_initialized == true)
        {
            return;
        }

        m_desc.physics.position = gameobject->transform().position;
        m_desc.physics.yawRadians = m_yawRadians;

        m_controller.init(world, m_desc.physics);
        m_initialized = true;
    }

    void PlayerControllerComponent::physicsShutdown()
    {
        if (m_initialized == false)
        {
            return;
        }

        m_controller.shutdown();
        m_initialized = false;
        m_debug.built = false;
    }

    void PlayerControllerComponent::fixedUpdate(float dt)
    {
        if (m_initialized == false)
        {
            return;
        }

        m_controller.setYaw(m_yawRadians);
        m_controller.update(m_pendingInput, dt);

        gameobject->transform().position = m_controller.getPosition();
        gameobject->transform().rotation.y = math::degrees(m_yawRadians);

        m_pendingInput.jumpPressed = false;
    }

    vec3 PlayerControllerComponent::getCameraLookTarget() const
    {
        return gameobject->transform().position + vec3(0.0f, m_eyeHeight, 0.0f);
    }

    vec3 PlayerControllerComponent::getThirdPersonCameraPosition() const
    {
        const vec3 target = getCameraLookTarget();

        const float sy = std::sin(m_yawRadians);
        const float cy = std::cos(m_yawRadians);
        const float sp = std::sin(m_pitchRadians);
        const float cp = std::cos(m_pitchRadians);

        // forward from yaw + pitch
        const vec3 forward(
            sy * cp,
            sp,
            cy * cp
        );

        return target - forward * m_cameraDistance;
    }

    vec3 PlayerControllerComponent::getCameraPosition() const
    {
        return gameobject->transform().position + vec3(0.0f, m_eyeHeight, 0.0f);
    }

    vec3 PlayerControllerComponent::getCameraForward() const
    {
        const float sy = std::sin(m_yawRadians);
        const float cy = std::cos(m_yawRadians);
        const float sp = std::sin(m_pitchRadians);
        const float cp = std::cos(m_pitchRadians);

        const vec3 forward(
            sy * cp,
            sp,
            cy * cp
        );

        return math::normalize(forward);
    }

}