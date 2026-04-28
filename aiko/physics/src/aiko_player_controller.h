#pragma once

#include <aiko_types.h>
#include <math/math_vector.h>

#include "aiko_query_types.h"
#include "player_controller_types.h"

namespace aiko::physics
{
    class AikoPhysics;
    class PlayerControllerImpl;

    class PlayerController
    {
    public:
        PlayerController();
        ~PlayerController();

        PlayerController(PlayerController&&) noexcept;
        PlayerController& operator=(PlayerController&&) noexcept;

        PlayerController(const PlayerController&) = delete;
        PlayerController& operator=(const PlayerController&) = delete;

        void init(AikoPhysics& physics, const PlayerControllerDesc& desc);
        void shutdown();

        void setPosition(const vec3& position);
        vec3 getPosition() const;

        void setYaw(float radians);
        float getYaw() const;

        void update(const PlayerInput& input, float dt);

        bool isOnGround() const;

        BodyMeshData getMeshData(bool worldSpace) const;

    private:
        AikoUPtr<PlayerControllerImpl> m_impl;
    };
}