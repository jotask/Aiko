
#pragma once

#include "vehicle_controls.h"

namespace ab
{
    class NetworkActionAdapter final
    {
    public:
        static VehicleControl toControls(float steering, float longitudinal);
    };
}
