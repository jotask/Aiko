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

    void applyPhysicsTransform(Transform& transform, const Transform& physicsTransform)
    {
        const Transform* parent = transform.getParent();

        if (parent == nullptr)
        {
            transform.position = physicsTransform.position;
            transform.rotation = physicsTransform.rotation;
            return;
        }

        transform.position = math::transformPoint(math::inverse(parent->getWorldMatrix()), physicsTransform.position);

        const JPH::Quat localRotation = worldRotation(*parent).Conjugated() * convert::toJoltRotation(physicsTransform.rotation);

        transform.rotation = convert::fromJoltRotation(localRotation.Normalized());
    }
}
