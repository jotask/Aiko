#pragma once

#include <vector>

#include "math/math.h"

namespace aiko
{
    struct Transform
    {
        Transform* parent = nullptr;
        std::vector<Transform*> childs;

        vec3 position = {0.0f};
        vec3 rotation = {0.0f};
        vec3 scale = {1.0f};

        mat4 getMatrix() const
        {
            const mat4 translationMatrix = math::translate(mat4(1.0f), position);

            // Create rotation matrix from rotation angles around each axis
            mat4 rotationMatrix = math::rotate(mat4(1.0f), math::radians(rotation.x), vec3(1.0f, 0.0f, 0.0f));
            rotationMatrix = math::rotate(rotationMatrix, math::radians(rotation.y), vec3(0.0f, 1.0f, 0.0f));
            rotationMatrix = math::rotate(rotationMatrix, math::radians(rotation.z), vec3(0.0f, 0.0f, 1.0f));

            // Create scale matrix from scale factors
            const mat4 scaleMatrix = math::scale(mat4(1.0f), scale);

            // Combine translation, rotation, and scale matrices to get the final transformation matrix
            const mat4 transformMatrix = rotationMatrix * translationMatrix * scaleMatrix;

            if (parent != nullptr)
            {
                return parent->getMatrix() * transformMatrix;
            }

            return transformMatrix;
        }

    };
}