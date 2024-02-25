#pragma once

#include <raylib.h>
#include "aiko_types.h"

namespace aiko
{

    class Utils
    {
    public:

        static Vector3 toV3(vec3 v)
        {
            return { v.x, v.y, v.z };
        }

    };

}