#include "network_action_adapter.h"

#include "math/math.h"

#include <algorithm>
#include <cmath>

namespace ab
{
    VehicleControl NetworkActionAdapter::toControls(float steering, float longitudinal)
    {
        VehicleControl control{};

        // Reject invalid network outputs.
        if (!aiko::math::isfinite(steering) || !aiko::math::isfinite(longitudinal))
        {
            return control;
        }

        // Steering: [-1, 1]
        control.steering = aiko::math::clamp(steering, -1.0f, 1.0f);

        // Longitudinal: [-1, 1]
        const float command = aiko::math::clamp(longitudinal, -1.0f, 1.0f);

        constexpr float DeadZone = 0.05f;

        // Positive output -> throttle
        if (command > DeadZone)
        {
            control.throttle = (command - DeadZone) / (1.0f - DeadZone);
        }
        // Negative output -> brake
        else if (command < -DeadZone)
        {
            control.brake = (-command - DeadZone) / (1.0f - DeadZone);
        }

        return control;
    }
}
