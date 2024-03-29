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

        mat4 translationMatrix = math::translate(mat4(1.0f), position);

        // Create rotation matrix from rotation angles around each axis
        mat4 rotationMatrix = math::rotate(mat4(1.0f), math::radians(rotation.x), vec3(1.0f, 0.0f, 0.0f));
        rotationMatrix = math::rotate(rotationMatrix, math::radians(rotation.y), vec3(0.0f, 1.0f, 0.0f));
        rotationMatrix = math::rotate(rotationMatrix, math::radians(rotation.z), vec3(0.0f, 0.0f, 1.0f));

        // Create scale matrix from scale factors
        mat4 scaleMatrix = math::scale(mat4(1.0f), scale);

        // Combine translation, rotation, and scale matrices to get the final transformation matrix
        mat4 transformMatrix = translationMatrix * rotationMatrix * scaleMatrix;

        return transformMatrix;
    }

}
