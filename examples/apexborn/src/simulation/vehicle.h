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

        void reset(const aiko::vec3& position, float heading, float speed = 0.0f);

        void setSteering(const float steering);
        void update(const float deltaTime);

        aiko::vec3 forwardDirection() const;

        const aiko::vec3& position() const { return m_position; }
        const float heading() const { return m_heading; }
        const float speed() const { return m_speed; }
        float steering() const { return m_steering; }

    private:
        aiko::vec3 m_position = {};
        float m_heading = 0.0f;
        float m_speed = 0.0;
        float m_steering = 0.0f;

        static constexpr float MaxTurnRate = std::numbers::pi_v<float> / 2.0f;
    };

}
