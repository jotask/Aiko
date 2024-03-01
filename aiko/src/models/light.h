#pragma once

#include "aiko_types.h"
#include <core/color.h>

namespace aiko
{
    
    class Light
    {
    public:
        
        enum Type
        {
            Ambient,
            Directional,
            Point,
        };
    
        Light();
        ~Light() = default;
    
        vec3 vector;
        Color color;
        float intensity;
    
        Type m_type = Type::Ambient;
    
        aiko::AikoPtr<vec3> m_light;
    
        void update();
    
        void reset();
    
    };

}
