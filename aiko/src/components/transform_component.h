#pragma once

#include <glm/glm.hpp>

#include "models/component.h"

class Transform : public Component
{
public:
    Transform();
    virtual ~Transform() = default;

    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);

};