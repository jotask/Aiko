#include "transform_component.h"

#include <GLFW/glfw3.h>

#include "shared/math_transform.h"

namespace aiko
{
    
    Transform::Transform()
        : Component("Transform")
    {
    
    }

    mat4 Transform::getMatrix()
    {
        mat4 tr = mat4(1.0f);
        tr = math::translate(tr, this->position);
        // FIXME temporal
        this->rotation = { 0.0f, 0.0f, 1.0f };
        tr = math::rotate(tr, (float)glfwGetTime(), this->rotation);
        return tr;
    }

}
