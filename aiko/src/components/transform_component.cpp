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

    mat4 Transform::getMatrix()
    {

        glm::mat4 tr = glm::mat4(1.0f);

        auto& p = this->position;
        glm::vec3 pos = { p.x, p.y, p.z };
        tr = glm::translate(tr, pos);

        // FIXME temporal
        this->rotation = { 0.0f, 0.0f, 1.0f };

        auto& r = this->rotation;
        glm::vec3 rot = { r.x, r.y, r.z };
        tr = glm::rotate(tr, (float)glfwGetTime(), rot);

        mat4 result;

        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                result.elements[i][j] = tr[i][j];
            }
        }

        return result;
    }

}
