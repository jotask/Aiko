#pragma once

#include "math/math.h"

#include <algorithm>

namespace aiko
{
    struct Transform
    {
        Transform* parent = nullptr;
        vector<Transform*> childs;

        vec3 position = {0.0f};
        vec3 rotation = {0.0f};
        vec3 scale = {1.0f};

        void setParent(Transform* newParent)
        {
            if (newParent == parent)
            {
                return;
            }

            if (parent != nullptr)
            {
                auto& siblings = parent->childs;
                siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
            }

            parent = newParent;

            if (parent != nullptr)
            {
                parent->childs.push_back(this);
            }
        }

        void clearParent()
        {
            setParent(nullptr);
        }

        void clearChildren()
        {
            while (childs.empty() == false)
            {
                childs.back()->clearParent();
            }
        }

        mat4 getLocalMatrix() const
        {
            const mat4 translationMatrix = math::translate(mat4(1.0f), position);
            mat4 rotationMatrix(1.0f);
            rotationMatrix = math::rotate(rotationMatrix, math::radians(rotation.x), vec3(1.0f, 0.0f, 0.0f));
            rotationMatrix = math::rotate(rotationMatrix, math::radians(rotation.y), vec3(0.0f, 1.0f, 0.0f));
            rotationMatrix = math::rotate(rotationMatrix, math::radians(rotation.z), vec3(0.0f, 0.0f, 1.0f));
            const mat4 scaleMatrix = math::scale(mat4(1.0f), scale);
            return translationMatrix * rotationMatrix * scaleMatrix;
        }

        mat4 getWorldMatrix() const
        {
            const mat4 localMatrix = getLocalMatrix();
            if (parent != nullptr)
            {
                return parent->getWorldMatrix() * localMatrix;
            }
            return localMatrix;
        }

    };
}