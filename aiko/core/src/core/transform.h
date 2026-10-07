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
            m_worldValid = false;

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
            refreshLocalMatrix();
            return m_localMatrix;
        }

        mat4 getWorldMatrix() const
        {
            refreshLocalMatrix();

            if (m_parent == nullptr)
            {
                if (m_worldValid == false || m_cachedLocalRevision != m_localRevision)
                {
                    m_worldMatrix = m_localMatrix;

                    m_cachedLocalRevision = m_localRevision;
                    m_cachedParentWorldRevision = 0;

                    m_worldValid = true;

                    ++m_worldRevision;
                }

                return m_worldMatrix;
            }

            const mat4 parentWorldMatrix = m_parent->getWorldMatrix();
            const u64 parentWorldRevision = m_parent->m_worldRevision;

            if (m_worldValid == false || m_cachedLocalRevision != m_localRevision || m_cachedParentWorldRevision != parentWorldRevision)
            {
                m_worldMatrix = parentWorldMatrix * m_localMatrix;

                m_cachedLocalRevision = m_localRevision;
                m_cachedParentWorldRevision = parentWorldRevision;

                m_worldValid = true;

                ++m_worldRevision;
            }

            return m_worldMatrix;
        }

    private:

        bool localTransformChanged() const
        {
            return
                position.x != m_cachedPosition.x ||
                position.y != m_cachedPosition.y ||
                position.z != m_cachedPosition.z ||

                rotation.x != m_cachedRotation.x ||
                rotation.y != m_cachedRotation.y ||
                rotation.z != m_cachedRotation.z ||

                scale.x != m_cachedScale.x ||
                scale.y != m_cachedScale.y ||
                scale.z != m_cachedScale.z;
        }

        void refreshLocalMatrix() const
        {
            if (localTransformChanged() == false)
            {
                return;
            }

            const mat4 translationMatrix = math::translate(mat4(1.0f), position);

            mat4 rotationMatrix(1.0f);

            rotationMatrix = math::rotate(rotationMatrix, math::radians(rotation.x), vec3(1.0f, 0.0f, 0.0f));
            rotationMatrix = math::rotate(rotationMatrix, math::radians(rotation.y), vec3(0.0f, 1.0f, 0.0f));
            rotationMatrix = math::rotate(rotationMatrix, math::radians(rotation.z), vec3(0.0f, 0.0f, 1.0f));

            const mat4 scaleMatrix = math::scale(mat4(1.0f), scale);

            m_localMatrix = translationMatrix * rotationMatrix * scaleMatrix;

            m_cachedPosition = position;
            m_cachedRotation = rotation;
            m_cachedScale = scale;

            ++m_localRevision;
        }


        Transform* m_parent = nullptr;
        vector<Transform*> m_children;

        mutable vec3 m_cachedPosition = {0.0f};
        mutable vec3 m_cachedRotation = {0.0f};
        mutable vec3 m_cachedScale = {1.0f};

        mutable mat4 m_localMatrix = mat4(1.0f);
        mutable mat4 m_worldMatrix = mat4(1.0f);

        mutable u64 m_localRevision = 0;
        mutable u64 m_worldRevision = 0;

        mutable u64 m_cachedLocalRevision = 0;
        mutable u64 m_cachedParentWorldRevision = 0;

        mutable bool m_worldValid = false;

    };
}