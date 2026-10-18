#include "physics_transform.h"

#include <Jolt/Jolt.h>
#include <Jolt/Math/Quat.h>

#include "internal/job_aiko_converters.h"

namespace aiko::physics
{
    namespace
    {
        JPH::Quat worldRotation(const Transform& transform)
        {
            JPH::Quat rotation = convert::toJoltRotation(transform.rotation);

            for (const Transform* parent = transform.getParent(); parent != nullptr; parent = parent->getParent())
            {
                rotation = convert::toJoltRotation(parent->rotation) * rotation;
            }

            return rotation.Normalized();
        }

    }

    Transform toPhysicsTransform(const Transform& transform)
    {
        Transform result{};
        result.position = math::transformPoint( transform.getWorldMatrix(), vec3(0.0f));

        result.rotation = transform.getParent() == nullptr ? transform.rotation : convert::fromJoltRotation(worldRotation(transform));

        return result;
    }

    bool applyPhysicsTransform(Transform& transform, const Transform& physicsTransform)
    {
        if (!math::isfinite(physicsTransform.position) || !math::isfinite(physicsTransform.rotation))
        {
            return false;
        }

        vec3 position = physicsTransform.position;
        vec3 rotation = physicsTransform.rotation;

        if (const Transform* parent = transform.getParent())
        {
            const mat4 parentWorld = parent->getWorldMatrix();

            const vec3 x{
                parentWorld(0, 0),
                parentWorld(1, 0),
                parentWorld(2, 0)
            };
            const vec3 y{
                parentWorld(0, 1),
                parentWorld(1, 1),
                parentWorld(2, 1)
            };
            const vec3 z{
                parentWorld(0, 2),
                parentWorld(1, 2),
                parentWorld(2, 2)
            };

            const float determinant = math::dot(x, math::cross(y, z));

            if (!math::isfinite(determinant) || determinant == 0.0f)
            {
                return false;
            }

            position = math::transformPoint(math::inverse(parentWorld), physicsTransform.position);

            if (!math::isfinite(position))
            {
                return false;
            }

            const JPH::Quat localRotation = worldRotation(*parent).Conjugated() * convert::toJoltRotation(physicsTransform.rotation);

            rotation = convert::fromJoltRotation(localRotation.Normalized());
        }

        if (!math::isfinite(rotation))
        {
            return false;
        }

        transform.position = position;
        transform.rotation = rotation;
        return true;
    }
}
