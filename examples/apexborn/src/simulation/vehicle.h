#pragma once

#include <math/math_vector.h>

#include <numbers>

namespace ab
{

    class Vehicle
    {
    public:
        Vehicle();
        ~Vehicle();

        void reset(const aiko::vec3& position, float heading);

        void setSteering(const float steering);
        void setThrottle(const float throttle);
        void setBrake(const float brake);

        void update(const float deltaTime);

        aiko::vec3 forwardDirection() const;

        const aiko::vec3& position() const { return m_position; }
        const float heading() const { return m_heading; }
        const float speed() const { return m_speed; }
        const float throttle() const { return m_throttle; }
        const float brake() const { return m_brake; }
        float steering() const { return m_steering; }

    private:

        static constexpr float MaxSpeed = 3.0f;
        static constexpr float MaxAcceleration = 1.5f;
        static constexpr float MaxBraking = 3.0f;
        static constexpr float RollingResistance = 0.2f;

        aiko::vec3 m_position = {};
        float m_heading = 0.0f;
        float m_speed = 0.0f;
        float m_steering = 0.0f;

        float m_throttle = 0.0f;
        float m_brake = 0.0f;

        static constexpr float MaxTurnRate = std::numbers::pi_v<float> / 2.0f;
    };

}
