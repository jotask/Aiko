#include "aiko_physics.h"

#include "internal/physics_impl.h"
#include "internal/jolt_bootstrap.h"
#include "internal/jolt_shape_factory.h"
#include "internal/job_aiko_converters.h"

#include <algorithm>
#include <array>
#include <thread>

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/MotionType.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/NarrowPhaseQuery.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>

namespace aiko::physics
{

    AikoPhysics::AikoPhysics() = default;

    AikoPhysics::~AikoPhysics()
    {
        shutdown();
    }

    AikoPhysics::AikoPhysics(AikoPhysics&&) noexcept = default;
    AikoPhysics& AikoPhysics::operator=(AikoPhysics&&) noexcept = default;

    void AikoPhysics::init(const WorldDesc& desc)
    {
        if (m_impl != nullptr)
        {
            return;
        }

        acquireJolt();

        m_impl = std::make_unique<PhysicsImpl>();

        m_impl->tempAllocator = std::make_unique<JPH::TempAllocatorImpl>(cTempAllocatorSize);

        const uint hwThreads = std::max(1u, std::thread::hardware_concurrency());
        const uint workerThreads = std::max(1u, hwThreads - 1u);

        m_impl->jobSystem = std::make_unique<JPH::JobSystemThreadPool>(
            JPH::cMaxPhysicsJobs,
            JPH::cMaxPhysicsBarriers,
            workerThreads
        );

        m_impl->physicsSystem = std::make_unique<JPH::PhysicsSystem>();
        m_impl->physicsSystem->Init(
            desc.maxBodies,
            desc.numBodyMutexes,
            desc.maxBodyPairs,
            desc.maxContactConstraints,
            m_impl->broadPhaseLayerInterface,
            m_impl->objectVsBroadPhaseLayerFilter,
            m_impl->objectLayerPairFilter
        );

        m_impl->physicsSystem->SetBodyActivationListener(&m_impl->activationListener);
        m_impl->physicsSystem->SetContactListener(&m_impl->contactListener);

    }

    void AikoPhysics::shutdown()
    {
        if (m_impl == nullptr)
        {
            return;
        }

        m_impl->physicsSystem->SetBodyActivationListener(nullptr);
        m_impl->physicsSystem->SetContactListener(nullptr);

        m_impl.reset();
        releaseJolt();
    }

    void AikoPhysics::step(const StepDesc& desc)
    {
        if (m_impl == nullptr) return;
        if (m_impl->physicsSystem == nullptr) return;
        if (m_impl->tempAllocator == nullptr) return;
        if (m_impl->jobSystem == nullptr) return;

        m_impl->physicsSystem->Update(
            desc.deltaTime,
            desc.collisionSteps,
            m_impl->tempAllocator.get(),
            m_impl->jobSystem.get()
        );
    }

    BodyId AikoPhysics::createBody(const BodyDesc& desc)
    {
        if (m_impl == nullptr || m_impl->physicsSystem == nullptr)
        {
            return InvalidBodyId;
        }

        JPH::BodyInterface& bodyInterface = m_impl->physicsSystem->GetBodyInterface();

        const JPH::RVec3 position = convert::toJoltVec3(desc.transform.position);
        const JPH::Quat rotation = convert::toJoltRotation(desc.transform.rotation);
        const JPH::EMotionType motionType = convert::toJoltMotionType(desc.motionType);
        const JPH::ObjectLayer layer = convert::toJoltLayer(desc.layer);

        JPH::RefConst<JPH::Shape> shape = createJoltShape(desc.shape);
        if (shape == nullptr)
        {
            return InvalidBodyId;
        }

        JPH::BodyCreationSettings settings(
            shape,
            position,
            rotation,
            motionType,
            layer
        );

        settings.mRestitution = desc.restitution;
        settings.mFriction = desc.friction;
        settings.mIsSensor = desc.isSensor;

        const JPH::EActivation activation =
            (desc.activate && desc.motionType != MotionType::Static)
                ? JPH::EActivation::Activate
                : JPH::EActivation::DontActivate;

        const JPH::BodyID bodyId = bodyInterface.CreateAndAddBody(settings, activation);
        return convert::toAikoBodyId(bodyId);
    }

    void AikoPhysics::destroyBody(BodyId id)
    {
        if (m_impl == nullptr || m_impl->physicsSystem == nullptr)
        {
            return;
        }

        if (id == InvalidBodyId)
        {
            return;
        }

        JPH::BodyInterface& bodyInterface = m_impl->physicsSystem->GetBodyInterface();
        const JPH::BodyID bodyId = convert::toJoltBodyId(id);

        bodyInterface.RemoveBody(bodyId);
        bodyInterface.DestroyBody(bodyId);
    }

    void AikoPhysics::setTransform(BodyId id, const Transform& transform)
    {
        if (m_impl == nullptr || m_impl->physicsSystem == nullptr)
        {
            return;
        }

        if (id == InvalidBodyId)
        {
            return;
        }

        JPH::BodyInterface& bodyInterface = m_impl->physicsSystem->GetBodyInterface();

        bodyInterface.SetPositionAndRotation(
            convert::toJoltBodyId(id),
            convert::toJoltVec3(transform.position),
            convert::toJoltRotation(transform.rotation),
            JPH::EActivation::Activate
        );
    }

    Transform AikoPhysics::getTransform(BodyId id) const
    {
        Transform result{};

        if (m_impl == nullptr || m_impl->physicsSystem == nullptr)
        {
            return result;
        }

        if (id == InvalidBodyId)
        {
            return result;
        }

        JPH::BodyInterface& bodyInterface = m_impl->physicsSystem->GetBodyInterface();

        JPH::RVec3 position;
        JPH::Quat rotation;
        bodyInterface.GetPositionAndRotation(convert::toJoltBodyId(id), position, rotation);

        result.position = convert::fromJoltVec3(position);
        result.rotation = convert::fromJoltRotation(rotation);
        result.scale = vec3(1.0f);

        return result;
    }

    void AikoPhysics::setLinearVelocity(BodyId id, const vec3& velocity)
    {
        if (m_impl == nullptr || m_impl->physicsSystem == nullptr)
        {
            return;
        }

        if (id == InvalidBodyId)
        {
            return;
        }

        JPH::BodyInterface& bodyInterface = m_impl->physicsSystem->GetBodyInterface();
        bodyInterface.SetLinearVelocity(convert::toJoltBodyId(id), convert::toJoltVec3(velocity));
    }

    vec3 AikoPhysics::getLinearVelocity(BodyId id) const
    {
        if (m_impl == nullptr || m_impl->physicsSystem == nullptr)
        {
            return vec3(0.0f);
        }

        if (id == InvalidBodyId)
        {
            return vec3(0.0f);
        }

        JPH::BodyInterface& bodyInterface = m_impl->physicsSystem->GetBodyInterface();
        return convert::fromJoltVec3(bodyInterface.GetLinearVelocity(convert::toJoltBodyId(id)));
    }

    void AikoPhysics::addImpulse(BodyId id, const vec3& impulse)
    {
        if (m_impl == nullptr || m_impl->physicsSystem == nullptr)
        {
            return;
        }

        if (id == InvalidBodyId)
        {
            return;
        }

        JPH::BodyInterface& bodyInterface = m_impl->physicsSystem->GetBodyInterface();
        bodyInterface.AddImpulse(convert::toJoltBodyId(id), convert::toJoltVec3(impulse));
    }

    void AikoPhysics::setAngularVelocity(BodyId id, const vec3& velocity)
    {
        if (m_impl == nullptr || m_impl->physicsSystem == nullptr)
        {
            return;
        }

        if (id == InvalidBodyId)
        {
            return;
        }

        JPH::BodyInterface& bodyInterface = m_impl->physicsSystem->GetBodyInterface();
        bodyInterface.SetAngularVelocity(convert::toJoltBodyId(id), convert::toJoltVec3(velocity));
    }

    vec3 AikoPhysics::getAngularVelocity(BodyId id) const
    {
        if (m_impl == nullptr || m_impl->physicsSystem == nullptr)
        {
            return vec3(0.0f);
        }

        if (id == InvalidBodyId)
        {
            return vec3(0.0f);
        }

        JPH::BodyInterface& bodyInterface = m_impl->physicsSystem->GetBodyInterface();
        return convert::fromJoltVec3(bodyInterface.GetAngularVelocity(convert::toJoltBodyId(id)));
    }

    void AikoPhysics::addForce(BodyId id, const vec3& force)
    {
        if (m_impl == nullptr || m_impl->physicsSystem == nullptr)
        {
            return;
        }

        if (id == InvalidBodyId)
        {
            return;
        }

        JPH::BodyInterface& bodyInterface = m_impl->physicsSystem->GetBodyInterface();
        bodyInterface.AddForce(convert::toJoltBodyId(id), convert::toJoltVec3(force));
    }

    void AikoPhysics::addTorque(BodyId id, const vec3& torque)
    {
        if (m_impl == nullptr || m_impl->physicsSystem == nullptr)
        {
            return;
        }

        if (id == InvalidBodyId)
        {
            return;
        }

        JPH::BodyInterface& bodyInterface = m_impl->physicsSystem->GetBodyInterface();
        bodyInterface.AddTorque(convert::toJoltBodyId(id), convert::toJoltVec3(torque));
    }

    void AikoPhysics::addAngularImpulse(BodyId id, const vec3& angularImpulse)
    {
        if (m_impl == nullptr || m_impl->physicsSystem == nullptr)
        {
            return;
        }

        if (id == InvalidBodyId)
        {
            return;
        }

        JPH::BodyInterface& bodyInterface = m_impl->physicsSystem->GetBodyInterface();
        bodyInterface.AddAngularImpulse(convert::toJoltBodyId(id), convert::toJoltVec3(angularImpulse));
    }

    void AikoPhysics::moveKinematic(BodyId id, const Transform& target, float deltaTime)
    {
        if (m_impl == nullptr || m_impl->physicsSystem == nullptr)
        {
            return;
        }

        if (id == InvalidBodyId)
        {
            return;
        }

        if (deltaTime <= 0.0f)
        {
            return;
        }

        JPH::BodyInterface& bodyInterface = m_impl->physicsSystem->GetBodyInterface();
        bodyInterface.MoveKinematic(
            convert::toJoltBodyId(id),
            convert::toJoltVec3(target.position),
            convert::toJoltRotation(target.rotation),
            deltaTime
        );
    }

    void AikoPhysics::setMotionType(BodyId id, MotionType motionType, bool activate)
    {
        if (m_impl == nullptr || m_impl->physicsSystem == nullptr)
        {
            return;
        }

        if (id == InvalidBodyId)
        {
            return;
        }

        JPH::BodyInterface& bodyInterface = m_impl->physicsSystem->GetBodyInterface();
        bodyInterface.SetMotionType(
            convert::toJoltBodyId(id),
            convert::toJoltMotionType(motionType),
            activate ? JPH::EActivation::Activate : JPH::EActivation::DontActivate
        );
    }

    MotionType AikoPhysics::getMotionType(BodyId id) const
    {
        if (m_impl == nullptr || m_impl->physicsSystem == nullptr)
        {
            return MotionType::Static;
        }

        if (id == InvalidBodyId)
        {
            return MotionType::Static;
        }

        JPH::BodyInterface& bodyInterface = m_impl->physicsSystem->GetBodyInterface();
        return convert::fromJoltMotionType(bodyInterface.GetMotionType(convert::toJoltBodyId(id)));
    }

    void AikoPhysics::setIsSensor(BodyId id, bool isSensor)
    {
        if (m_impl == nullptr || m_impl->physicsSystem == nullptr)
        {
            return;
        }

        if (id == InvalidBodyId)
        {
            return;
        }

        JPH::BodyInterface& bodyInterface = m_impl->physicsSystem->GetBodyInterface();
        bodyInterface.SetIsSensor(convert::toJoltBodyId(id), isSensor);
    }

    bool AikoPhysics::isSensor(BodyId id) const
    {
        if (m_impl == nullptr || m_impl->physicsSystem == nullptr)
        {
            return 1.0f;
        }

        if (id == InvalidBodyId)
        {
            return 1.0f;
        }

        JPH::BodyInterface& bodyInterface = m_impl->physicsSystem->GetBodyInterface();
        return bodyInterface.IsSensor(convert::toJoltBodyId(id));
    }

    void AikoPhysics::setGravityFactor(BodyId id, float gravityFactor)
    {
        if (m_impl == nullptr || m_impl->physicsSystem == nullptr)
        {
            return;
        }

        if (id == InvalidBodyId)
        {
            return;
        }

        JPH::BodyInterface& bodyInterface = m_impl->physicsSystem->GetBodyInterface();
        bodyInterface.SetGravityFactor(convert::toJoltBodyId(id), gravityFactor);
    }

    float AikoPhysics::getGravityFactor(BodyId id) const
    {
        if (m_impl == nullptr || m_impl->physicsSystem == nullptr)
        {
            return 1.0f;
        }

        if (id == InvalidBodyId)
        {
            return 1.0f;
        }

        JPH::BodyInterface& bodyInterface = m_impl->physicsSystem->GetBodyInterface();
        return bodyInterface.GetGravityFactor(convert::toJoltBodyId(id));
    }

    BodyMeshData AikoPhysics::getBodyMeshData(BodyId id, bool worldSpace) const
    {
        BodyMeshData out{};

        if (m_impl == nullptr || m_impl->physicsSystem == nullptr)
        {
            return out;
        }

        if (id == InvalidBodyId)
        {
            return out;
        }

        JPH::BodyInterface& bodyInterface = m_impl->physicsSystem->GetBodyInterface();
        const JPH::BodyID bodyId = convert::toJoltBodyId(id);

        JPH::RefConst<JPH::Shape> shape = bodyInterface.GetShape(bodyId);
        if (shape == nullptr)
        {
            return out;
        }

        JPH::RVec3 position = JPH::RVec3::sZero();
        JPH::Quat rotation = JPH::Quat::sIdentity();

        if (worldSpace == true)
        {
            bodyInterface.GetPositionAndRotation(bodyId, position, rotation);
        }

        JPH::Shape::GetTrianglesContext context;

        // Large bounds so we fetch the whole shape.
        // Good enough for debug / generic mesh extraction.
        const JPH::AABox bounds(
            JPH::Vec3(-100000.0f, -100000.0f, -100000.0f),
            JPH::Vec3( 100000.0f,  100000.0f,  100000.0f)
        );

        shape->GetTrianglesStart(
            context,
            bounds,
            position,
            rotation,
            JPH::Vec3::sReplicate(1.0f)
        );

        constexpr int kBatch = JPH::Shape::cGetTrianglesMinTrianglesRequested;
        std::array<JPH::Float3, kBatch * 3> triangles{};

        for (;;)
        {
            const int triangleCount = shape->GetTrianglesNext(
                context,
                kBatch,
                triangles.data(),
                nullptr
            );

            if (triangleCount == 0)
            {
                break;
            }

            for (int t = 0; t < triangleCount; ++t)
            {
                const u32 baseIndex = static_cast<u32>(out.vertices.size());

                for (int v = 0; v < 3; ++v)
                {
                    const JPH::Float3& p = triangles[t * 3 + v];
                    out.vertices.push_back(vec3(p.x, p.y, p.z));
                }

                out.indices.push_back(baseIndex + 0);
                out.indices.push_back(baseIndex + 1);
                out.indices.push_back(baseIndex + 2);
            }
        }

        return out;
    }

    bool AikoPhysics::castRay(const RayCastDesc& desc, RayCastHit& outHit) const
    {
        outHit = {};

        if (m_impl == nullptr || m_impl->physicsSystem == nullptr)
        {
            return false;
        }

        const vec3 delta = desc.to - desc.from;
        if (delta.x == 0.0f && delta.y == 0.0f && delta.z == 0.0f)
        {
            return false;
        }

        const JPH::RRayCast ray(
            convert::toJoltVec3(desc.from),
            JPH::Vec3(delta.x, delta.y, delta.z)
        );

        JPH::RayCastResult hit;
        const bool hasHit = m_impl->physicsSystem->GetNarrowPhaseQuery().CastRay(ray, hit);
        if (hasHit == false)
        {
            return false;
        }

        const JPH::Vec3 hitPoint = ray.GetPointOnRay(hit.mFraction);

        outHit.body = convert::toAikoBodyId(hit.mBodyID);
        outHit.fraction = hit.mFraction;
        outHit.position = vec3(hitPoint.GetX(), hitPoint.GetY(), hitPoint.GetZ());

        const JPH::BodyLockInterface& lockInterface = m_impl->physicsSystem->GetBodyLockInterface();
        JPH::BodyLockRead bodyLock(lockInterface, hit.mBodyID);

        if (bodyLock.Succeeded() == true)
        {
            const JPH::Body& body = bodyLock.GetBody();
            const JPH::Vec3 normal = body.GetWorldSpaceSurfaceNormal(hit.mSubShapeID2, hitPoint);

            outHit.normal = vec3(
                normal.GetX(),
                normal.GetY(),
                normal.GetZ()
            );
        }

        return true;
    }

    vector<PhysicsEvent> AikoPhysics::drainEvents()
    {
        if (m_impl == nullptr)
        {
            return {};
        }
        return m_impl->eventQueue.drain();
    }
}
