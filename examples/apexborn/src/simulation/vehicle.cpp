#include "vehicle.h"

#include "math/math_transform.h"

namespace ab
{

    Vehicle::Vehicle() = default;

    Vehicle::~Vehicle() = default;

    void Vehicle::reset(const aiko::vec3& position, float heading)
    {
        m_position = position;
        m_heading = heading;
        m_speed = 0.0f;
        m_steering = 0.0f;
        m_throttle = 0.0f;
        m_brake = 0.0f;
    }

    void Vehicle::setSteering(const float steering)
    {
        if (aiko::math::isfinite(steering) == false)
        {
            return;
        }
        m_steering = aiko::math::clamp(steering, -1.0f, 1.0f);
    }

    void Vehicle::setThrottle(const float throttle)
    {
        if (aiko::math::isfinite(throttle) == false)
        {
            return;
        }
        m_throttle = aiko::math::clamp(throttle, 0.0f, 1.0f);
    }

    void Vehicle::setBrake(const float brake)
    {
        if (aiko::math::isfinite(brake) == false)
        {
            return;
        }
        m_brake = aiko::math::clamp(brake, 0.0f, 1.0f);
    }

    void Vehicle::update(const float deltaTime)
    {

        const auto effectiveThrottle = m_brake > 0.0f ? 0.0f : m_throttle;
        const auto acceleration = (effectiveThrottle * MaxAcceleration) - (m_brake * MaxBraking) - RollingResistance;
        m_speed = aiko::math::clamp(m_speed + acceleration * deltaTime, 0.0f, MaxSpeed);

        m_heading += m_steering * MaxTurnRate * deltaTime;
        m_position = m_position + forwardDirection() * m_speed * deltaTime;
    }

    aiko::vec3 Vehicle::forwardDirection() const
    {
        return { aiko::math::cos(m_heading), 0.0f, aiko::math::sin(m_heading) };
    }

}
