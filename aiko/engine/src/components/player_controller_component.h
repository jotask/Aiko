#pragma once

#include <aiko_player_controller.h>

#include "models/component.h"
#include "components/physics_debug_proxy.h"

namespace aiko
{
    struct PlayerControllerComponentDesc
    {
        physics::PlayerControllerDesc physics{};
        float eyeHeight = 1.6f;
        float pitchRadians = 0.0f;
        bool debugDraw = false;
    };

    class PlayerControllerComponent : public Component
    {
    public:
        PlayerControllerComponent();
        ~PlayerControllerComponent() override;

        void create(const PlayerControllerComponentDesc& desc);
        void dispose() override;

        void setMoveInput(const vec2& move);
        void setJumpPressed(bool pressed);
        void addLookDelta(float yawDelta, float pitchDelta);

        vec3 getEyePosition() const;
        float getYaw() const;
        float getPitch() const;

        bool isPhysicsInitialized() const { return m_initialized; }
        const Transform& getWorldTransform() const;

        physics::PlayerController& getController() { return m_controller; }
        const physics::PlayerController& getController() const { return m_controller; }

        PhysicsDebugProxy& debug() { return m_debug; }
        const PhysicsDebugProxy& debug() const { return m_debug; }

        void ensurePhysicsInitialized(physics::AikoPhysics& world);
        void physicsShutdown();
        void fixedUpdate(float dt);

        vec3 getThirdPersonCameraPosition() const;
        vec3 getCameraLookTarget() const;

        vec3 getCameraPosition() const;
        vec3 getCameraForward() const;

    private:
        PlayerControllerComponentDesc m_desc{};
        physics::PlayerController m_controller;
        physics::PlayerInput m_pendingInput{};

        float m_cameraDistance = 4.0f;
        float m_cameraHeight = 1.6f;

        float m_yawRadians = 0.0f;
        float m_pitchRadians = 0.0f;
        float m_eyeHeight = 1.6f;

        bool m_initialized = false;
        PhysicsDebugProxy m_debug{};
    };
}