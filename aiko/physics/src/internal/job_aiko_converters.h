#pragma once

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/MotionType.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/NarrowPhaseQuery.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>

#include "internal/jolt_layers.h"
#include "aiko_physics_types.h"

#include <math/math.h>

#include "Jolt/Physics/Character/CharacterID.h"

namespace aiko::physics
{

    namespace convert
    {

        inline JPH::Vec3 toJoltVec3(const vec3& v)
        {
            return JPH::Vec3(v.x, v.y, v.z);
        }

        inline vec3 fromJoltVec3(JPH::Vec3Arg v)
        {
            return vec3(
                static_cast<float>(v.GetX()),
                static_cast<float>(v.GetY()),
                static_cast<float>(v.GetZ())
            );
        }

        inline JPH::Quat toJoltRotation(const vec3& eulerDegrees)
        {
            return JPH::Quat::sEulerAngles(JPH::Vec3(
                math::radians(eulerDegrees.x),
                math::radians(eulerDegrees.y),
                math::radians(eulerDegrees.z)
            ));
        }

        inline vec3 fromJoltRotation(JPH::QuatArg q)
        {
            const JPH::Vec3 eulerRadians = q.GetEulerAngles();

            return vec3(
                math::degrees(eulerRadians.GetX()),
                math::degrees(eulerRadians.GetY()),
                math::degrees(eulerRadians.GetZ())
            );
        }

        inline JPH::Quat toYawRotation(float yawRadians)
        {
            return JPH::Quat::sRotation(JPH::Vec3::sAxisY(), yawRadians);
        }

        inline JPH::EMotionType toJoltMotionType(MotionType type)
        {
            switch (type)
            {
                case MotionType::Static:    return JPH::EMotionType::Static;
                case MotionType::Kinematic: return JPH::EMotionType::Kinematic;
                case MotionType::Dynamic:   return JPH::EMotionType::Dynamic;
                default:                    return JPH::EMotionType::Static;
            }
        }

        inline JPH::ObjectLayer toJoltLayer(ObjectLayer layer)
        {
            switch (layer)
            {
                case ObjectLayer::NonMoving: return Layers::NON_MOVING;
                case ObjectLayer::Moving:    return Layers::MOVING;
                default:                     return Layers::NON_MOVING;
            }
        }

        inline MotionType fromJoltMotionType(JPH::EMotionType type)
        {
            switch (type)
            {
            case JPH::EMotionType::Static:    return MotionType::Static;
            case JPH::EMotionType::Kinematic: return MotionType::Kinematic;
            case JPH::EMotionType::Dynamic:   return MotionType::Dynamic;
            default:                          return MotionType::Static;
            }
        }

        inline BodyId toAikoBodyId(const JPH::BodyID& id)
        {
            return id.IsInvalid() ? InvalidBodyId : id.GetIndexAndSequenceNumber();
        }

        inline JPH::BodyID toJoltBodyId(BodyId id)
        {
            return JPH::BodyID(id);
        }

        inline JPH::CharacterID toJoltCharacterId(CharacterId id)
        {
            return JPH::CharacterID(id);
        }
    }

}
