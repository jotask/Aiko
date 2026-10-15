#include "player_controller_component.h"

#include "models/game_object.h"

#include <physics_transform.h>

namespace aiko
{
    PlayerControllerComponent::PlayerControllerComponent()
        : Component("PlayerControllerComponent")
    {
    }

    PlayerControllerComponent::~PlayerControllerComponent() = default;

    void PlayerControllerComponent::create(const PlayerControllerComponentDesc& desc)
    {
        physicsShutdown();

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
        const vec3 position = m_initialized ? m_controller.getPosition() : physics::toPhysicsTransform(gameobject->transform()).position;
        return position + vec3(0.0f, m_eyeHeight, 0.0f);
    }

    float PlayerControllerComponent::getYaw() const
    {
        return m_yawRadians;
    }

    float PlayerControllerComponent::getPitch() const
    {
        return m_pitchRadians;
    }

    Transform PlayerControllerComponent::getWorldTransform() const
    {
        Transform pose = physics::toPhysicsTransform(gameobject->transform());

        if (m_initialized)
        {
            pose.position = m_controller.getPosition();
        }

        pose.rotation = vec3(0.0f, math::degrees(m_yawRadians), 0.0f);
        return pose;
    }

    void PlayerControllerComponent::ensurePhysicsInitialized(physics::AikoPhysics& world)
    {
        if (m_initialized == true)
        {
            return;
        }

        const Transform pose = physics::toPhysicsTransform(gameobject->transform());

        m_desc.physics.position = pose.position;
        m_desc.physics.yawRadians = m_yawRadians;

        m_initialized = m_controller.init(world, m_desc.physics);
        AIKO_ASSERT(m_initialized, "PlayerController creation failed");
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

        Transform pose{};
        pose.position = m_controller.getPosition();
        pose.rotation.y = math::degrees(m_yawRadians);

        physics::applyPhysicsTransform(gameobject->transform(), pose);

        m_pendingInput.jumpPressed = false;
    }

    vec3 PlayerControllerComponent::getCameraLookTarget() const
    {
        return getEyePosition();
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
        return getEyePosition();
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