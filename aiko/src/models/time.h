#pragma once

#include <glm/glm.hpp>

#include "aiko_types.h"

namespace aiko
{
    
    class Time
    {
    public:
        
        enum Type
        {
            Ambient,
            Directional,
            Point,
        };
    
        Time();
        ~Time() = default;
    
    };

}
