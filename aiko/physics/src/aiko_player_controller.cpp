#include "aiko_player_controller.h"

#include "aiko_physics.h"
#include "internal/physics_impl.h"
#include "internal/jolt_layers.h"
#include "internal/player_controller_impl.h"
#include "internal/job_aiko_converters.h"

#include <cmath>

#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Math/Quat.h>
#include <Jolt/Physics/Body/BodyFilter.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Character/Character.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>

namespace aiko::physics
{

    PlayerController::PlayerController() = default;
    PlayerController::~PlayerController() = default;

    PlayerController::PlayerController(PlayerController&&) noexcept = default;
    PlayerController& PlayerController::operator=(PlayerController&&) noexcept = default;

    void PlayerController::init(AikoPhysics& physics, const PlayerControllerDesc& desc)
    {
        if (m_impl != nullptr)
        {
            return;
        }

        AIKO_ASSERT(physics.m_impl != nullptr, "AikoPhysics must be initialized before PlayerController");
        AIKO_ASSERT(physics.m_impl->physicsSystem != nullptr, "PhysicsSystem is null");
        AIKO_ASSERT(physics.m_impl->tempAllocator != nullptr, "TempAllocator is null");

        m_impl = std::make_unique<PlayerControllerImpl>();
        m_impl->world = &physics;
        m_impl->worldImpl = physics.m_impl.get();
        m_impl->desc = desc;
        m_impl->yawRadians = desc.yawRadians;

        JPH::CharacterVirtualSettings settings;
        settings.mUp = JPH::Vec3::sAxisY();
        settings.mMaxSlopeAngle = JPH::DegreesToRadians(desc.maxSlopeAngleDegrees);
        settings.mCharacterPadding = desc.characterPadding;
        settings.mPredictiveContactDistance = desc.predictiveContactDistance;
        settings.mMass = desc.mass;
        settings.mMaxStrength = desc.maxStrength;
        settings.mEnhancedInternalEdgeRemoval = true;
        settings.mInnerBodyLayer = Layers::MOVING;
        settings.mShape = new JPH::CapsuleShape(desc.halfHeight, desc.radius);

        m_impl->character = new JPH::CharacterVirtual(
            &settings,
            convert::toJoltVec3(desc.position),
            convert::toYawRotation(desc.yawRadians),
            desc.userData,
            m_impl->worldImpl->physicsSystem.get()
        );
    }

    void PlayerController::shutdown()
    {
        m_impl.reset();
    }

    void PlayerController::setPosition(const vec3& position)
    {
        if (m_impl == nullptr || m_impl->character == nullptr)
        {
            return;
        }

        m_impl->character->SetPosition(convert::toJoltVec3(position));
    }

    vec3 PlayerController::getPosition() const
    {
        if (m_impl == nullptr || m_impl->character == nullptr)
        {
            return vec3(0.0f);
        }

        return convert::fromJoltVec3(m_impl->character->GetPosition());
    }

    void PlayerController::setYaw(float radians)
    {
        if (m_impl == nullptr)
        {
            return;
        }

        m_impl->yawRadians = radians;

        if (m_impl->character != nullptr)
        {
            m_impl->character->SetRotation(convert::toYawRotation(radians));
        }

    }

    float PlayerController::getYaw() const
    {
        if (m_impl == nullptr)
        {
            return 0.0f;
        }

        return m_impl->yawRadians;
    }

    bool PlayerController::isOnGround() const
    {
        if (m_impl == nullptr || m_impl->character == nullptr)
        {
            return false;
        }

        return m_impl->character->IsSupported();
    }

    BodyMeshData PlayerController::getMeshData(bool worldSpace) const
    {

        BodyMeshData out = {};

        if (m_impl == nullptr || m_impl->character == nullptr)
        {
            return out;
        }

        const JPH::Shape* shape = m_impl->character->GetShape();
        if (shape == nullptr)
        {
            return out;
        }

        JPH::RVec3 position = JPH::RVec3::sZero();
        JPH::Quat rotation = JPH::Quat::sIdentity();

        if (worldSpace)
        {
            position = m_impl->character->GetPosition();
            rotation = m_impl->character->GetRotation();
        }

        JPH::Shape::GetTrianglesContext context;

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

    void PlayerController::update(const PlayerInput& input, float dt)
    {

        if (m_impl == nullptr || m_impl->character == nullptr || dt <= 0.0f)
        {
            return;
        }

        const float sy = std::sin(m_impl->yawRadians);
        const float cy = std::cos(m_impl->yawRadians);

        const vec3 forward = vec3(sy, 0.0f, cy);
        const vec3 right   = vec3(cy, 0.0f, -sy);

        vec3 move = right * input.move.x + forward * input.move.y;
        const float moveLenSq = move.x * move.x + move.y * move.y + move.z * move.z;
        if (moveLenSq > 1.0f)
        {
            const float invLen = 1.0f / std::sqrt(moveLenSq);
            move *= invLen;
        }

        move *= m_impl->desc.walkSpeed;

        const JPH::Vec3 gravity(0.0f, -9.81f, 0.0f);

        JPH::Vec3 currentVelocity = m_impl->character->GetLinearVelocity();
        JPH::Vec3 newVelocity = currentVelocity;

        // horizontal movement from input
        newVelocity.SetX(move.x);
        newVelocity.SetZ(move.z);

        // gravity affects vertical velocity every frame
        newVelocity += gravity * dt;

        // jump overrides vertical velocity when grounded
        if (input.jumpPressed && m_impl->character->IsSupported())
        {
            newVelocity.SetY(m_impl->desc.jumpSpeed);
        }

        m_impl->character->SetRotation(convert::toYawRotation(m_impl->yawRadians));
        m_impl->character->SetLinearVelocity(newVelocity);

        const JPH::DefaultBroadPhaseLayerFilter broadPhaseFilter(
            m_impl->worldImpl->objectVsBroadPhaseLayerFilter,
            Layers::MOVING
        );

        const JPH::DefaultObjectLayerFilter objectLayerFilter(
            m_impl->worldImpl->objectLayerPairFilter,
            Layers::MOVING
        );

        const JPH::BodyFilter bodyFilter;
        const JPH::ShapeFilter shapeFilter;

        JPH::CharacterVirtual::ExtendedUpdateSettings updateSettings;

        m_impl->character->ExtendedUpdate(
            dt,
            gravity,
            updateSettings,
            broadPhaseFilter,
            objectLayerFilter,
            bodyFilter,
            shapeFilter,
            *m_impl->worldImpl->tempAllocator
        );
    }

}