#pragma once

#include <glm/glm.hpp>

#include "aiko_types.h"

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

    glm::vec3 vector;
    glm::vec4 color;
    float intensity;

    Type m_type = Type::Ambient;

    aiko::AikoPtr<glm::vec3> m_light;

    void update();

    void reset();

};