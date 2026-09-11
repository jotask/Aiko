#pragma once

#include "aiko_types.h"
#include "math/math.h"

#include <algorithm>

namespace aiko
{
    struct Transform
    {
    public:
        vec3 position = {0.0f};
        vec3 rotation = {0.0f};
        vec3 scale = {1.0f};

        void setParent(Transform* newParent)
        {
            AIKO_ASSERT(newParent != this, "Transform cannot be parented to itself");

            if (newParent == m_parent)
            {
                return;
            }

            for (Transform* ancestor = newParent; ancestor != nullptr; ancestor = ancestor->m_parent)
            {
                AIKO_ASSERT(ancestor != this, "Transform hierarchy cannot contain cycles");
            }

            if (m_parent != nullptr)
            {
                auto& siblings = m_parent->m_children;
                siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
            }

            m_parent = newParent;

            if (m_parent != nullptr)
            {
                m_parent->m_children.push_back(this);
            }
        }

        void clearParent()
        {
            setParent(nullptr);
        }

        void clearChildren()
        {
            while (m_children.empty() == false)
            {
                m_children.back()->clearParent();
            }
        }

        Transform* getParent() const
        {
            return m_parent;
        }

        const vector<Transform*>& getChildren() const
        {
            return m_children;
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
            if (m_parent != nullptr)
            {
                return m_parent->getWorldMatrix() * localMatrix;
            }
            return localMatrix;
        }

    private:
        Transform* m_parent = nullptr;
        vector<Transform*> m_children;

    };
}