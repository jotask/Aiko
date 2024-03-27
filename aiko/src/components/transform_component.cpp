#include "transform_component.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace aiko
{
    
    Transform::Transform()
        : Component("Transform")
    {
    
    }

    auto Transform::getMatrix()
    {
        // create transformations
        glm::mat4 tr = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

        auto& p = glm::vec3(0.5f, -0.5f, 0.0f);// transform->position;
        glm::vec3 pos = { p.x, p.y, p.z };
        tr = glm::translate(tr, glm::vec3(0.5f, -0.5f, 0.0f));

        auto& r = glm::vec3(0.0f, 0.0f, 1.0f); // transform->rotation;
        glm::vec3 rot = { r.x, r.y, r.z };
        tr = glm::rotate(tr, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

        return tr;
    }

}
