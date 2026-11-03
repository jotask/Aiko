#include "vehicle.h"

#include "math/math_transform.h"

namespace ab
{

    Vehicle::Vehicle() = default;

    Vehicle::~Vehicle() = default;

    void Vehicle::reset(const aiko::vec3& position, float heading, float speed)
    {
        m_position = position;
        m_heading = heading;
        m_speed = speed;
        m_steering = 0.0f;
    }

    void Vehicle::setSteering(const float steering)
    {
        m_steering = aiko::math::clamp(steering, -1.0f, 1.0f);
    }

    void Vehicle::update(const float deltaTime)
    {
        m_heading += m_steering * MaxTurnRate * deltaTime;
        m_position = m_position + forwardDirection() * m_speed * deltaTime;
    }

    aiko::vec3 Vehicle::forwardDirection() const
    {
        return { aiko::math::cos(m_heading), 0.0f, aiko::math::sin(m_heading) };
    }

}
