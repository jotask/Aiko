#include "jolt_ragdoll_factory.h"

#include "job_aiko_converters.h"
#include "jolt_shape_factory.h"
#include "jolt_layers.h"

#include <Jolt/Physics/Ragdoll/Ragdoll.h>
#include <Jolt/Skeleton/Skeleton.h>

namespace aiko::physics
{

    JPH::Ref<JPH::RagdollSettings> JoltRagdollFactory::createSettings(const RagdollDesc& desc)
    {
        if (desc.empty())
        {
            return {};
        }

        JPH::Ref<JPH::Skeleton> skeleton = new JPH::Skeleton();
        JPH::Ref<JPH::RagdollSettings> settings = new JPH::RagdollSettings();

        settings->mSkeleton = skeleton;
        settings->mParts.resize(desc.size());

        for (std::size_t i = 0; i < desc.size(); ++i)
        {
            const RagdollPartDesc& part = desc[i];
            if ((i == 0 && part.parentIndex != -1) || (i > 0 && (part.parentIndex < 0 || part.parentIndex >= static_cast<int>(i))))
            {
                return {};
            }
            skeleton->AddJoint(part.name, part.parentIndex);
        }

        for (std::size_t i = 0; i < desc.size(); ++i)
        {
            const RagdollPartDesc& partDesc = desc[i];
            JPH::RagdollSettings::Part& part = settings->mParts[i];

            JPH::RefConst<JPH::Shape> shape = createJoltShape(partDesc.shape);
            if (shape == nullptr)
            {
                return {};
            }

            part.SetShape(shape);

            const aiko::vec3& position = partDesc.transform.position;

            part.mPosition = JPH::RVec3(position.x, position.y, position.z);
            part.mRotation = convert::toJoltRotation(partDesc.transform.rotation);

            part.mMotionType = JPH::EMotionType::Kinematic;
            part.mObjectLayer = Layers::MOVING;

        }

        settings->CalculateBodyIndexToConstraintIndex();
        settings->CalculateConstraintIndexToBodyIdxPair();

        return settings;
    }

}
