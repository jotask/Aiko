#include "physics_system.h"

#include "render_system.h"
#include "system_connector.h"
#include "models/mesh_factory.h"
#include "time/time.h"

namespace aiko
{
    PhysicsSystem::PhysicsSystem() = default;

    void PhysicsSystem::connect(ModuleConnector*, SystemConnector* systemConnector)
    {
        BIND_SYSTEM_REQUIRED(RenderSystem, systemConnector, m_renderSystem);
    }

    void PhysicsSystem::init()
    {
        m_physics.init();

        m_debugMaterial.m_shaderId = aiko->getComponentAssetAccess()->registerShader("model");
        m_debugMaterial.m_baseColor = MAGENTA;
        m_debugMaterial.m_useVertexColor = true;
        m_debugMaterial.m_lit = false;
    }

    void PhysicsSystem::update()
    {
        BaseSystem::update();

        m_physicsAccumulator += Time::it().getDeltaTime();
        constexpr int kMaxSubSteps = 4;

        int subSteps = 0;
        while (m_physicsAccumulator >= physics::kPhysicsDeltaTime && subSteps < kMaxSubSteps)
        {
            for (RigidBodyComponent* body : m_rigidBodies)
            {
                if (body != nullptr)
                {
                    body->ensurePhysicsInitialized(m_physics);
                }
            }

            for (PlayerControllerComponent* controller : m_playerControllers)
            {
                if (controller != nullptr)
                {
                    controller->ensurePhysicsInitialized(m_physics);
                }
            }

            for (PlayerControllerComponent* controller : m_playerControllers)
            {
                if (controller != nullptr)
                {
                    controller->fixedUpdate(physics::kPhysicsDeltaTime);
                }
            }

            const physics::StepDesc stepDesc
            {
                .deltaTime = physics::kPhysicsDeltaTime,
                .collisionSteps = 1,
            };

            m_physics.step(stepDesc);

            for (RigidBodyComponent* body : m_rigidBodies)
            {
                if (body != nullptr)
                {
                    body->syncFromPhysics(m_physics);
                }
            }

            m_physicsAccumulator -= physics::kPhysicsDeltaTime;
            ++subSteps;
        }

        for (RigidBodyComponent* body : m_rigidBodies)
        {
            ensureDebugMesh(body);
        }

        for (PlayerControllerComponent* controller : m_playerControllers)
        {
            ensureDebugMesh(controller);
        }

        const vector<physics::PhysicsEvent> events = m_physics.drainEvents();
        for (const physics::PhysicsEvent& event : events)
        {
            switch (event.type)
            {
                case physics::PhysicsEventType::BodyActivated:
                    logger::Log::trace("Body activated: {}", event.bodyA);
                    break;
                case physics::PhysicsEventType::BodyDeactivated:
                    logger::Log::trace("Body deactivated: {}", event.bodyA);
                    break;
                case physics::PhysicsEventType::ContactAdded:
                    logger::Log::trace("Contact added: {} <-> {}", event.bodyA, event.bodyB);
                    break;
                case physics::PhysicsEventType::ContactRemoved:
                    logger::Log::trace("Contact removed: {} <-> {}", event.bodyA, event.bodyB);
                    break;
            }
        }
    }

    void PhysicsSystem::render()
    {
        BaseSystem::render();

        for (RigidBodyComponent* body : m_rigidBodies)
        {
            if (body != nullptr && body->debug().enabled && body->debug().built)
            {
                Transform debugTransform = body->getWorldTransform();
                debugTransform.scale *= 1.01f;
                m_renderSystem->render(debugTransform, body->debug().mesh, m_debugMaterial);
            }
        }

        for (PlayerControllerComponent* controller : m_playerControllers)
        {
            if (controller != nullptr && controller->debug().enabled && controller->debug().built)
            {
                Transform debugTransform = controller->getWorldTransform();
                debugTransform.scale *= 1.01f;
                m_renderSystem->render(debugTransform, controller->debug().mesh, m_debugMaterial);
            }
        }
    }

    void PhysicsSystem::dispose()
    {
        for (RigidBodyComponent* body : m_rigidBodies)
        {
            if (body != nullptr)
            {
                body->physicsShutdown(m_physics);
            }
        }

        for (PlayerControllerComponent* controller : m_playerControllers)
        {
            if (controller != nullptr)
            {
                controller->physicsShutdown();
            }
        }

        m_rigidBodies.clear();
        m_playerControllers.clear();

        m_physics.shutdown();
        BaseSystem::dispose();
    }

    MeshAsset PhysicsSystem::makeMeshAssetFromPhysics(const physics::BodyMeshData& data)
    {
        MeshAsset asset{};

        AIKO_ASSERT(
            data.vertices.size() <= std::numeric_limits<uint16_t>::max(),
            "Physics mesh is too large for MeshAsset uint16_t indices"
        );

        asset.m_vertices = data.vertices;
        asset.m_textCoord.assign(data.vertices.size(), vec2(0.0f, 0.0f));
        asset.m_colors.assign(data.vertices.size(), MAGENTA);

        asset.m_indices.reserve(data.indices.size());
        for (u32 index : data.indices)
        {
            asset.m_indices.push_back(static_cast<uint16_t>(index));
        }

        mesh::factory::recalculateNormals(asset);
        return asset;
    }

    void PhysicsSystem::ensureDebugMesh(RigidBodyComponent* component)
    {
        if (component == nullptr)
        {
            return;
        }

        if (component->debug().enabled == false || component->debug().built == true || component->isPhysicsInitialized() == false)
        {
            return;
        }

        const physics::BodyMeshData meshData = m_physics.getBodyMeshData(component->getBodyId(), false);

        logger::Log::trace(
            "Rigid debug mesh build: body={%zu}, verts={%zu}, indices={%zu}",
            component->getBodyId(),
            meshData.vertices.size(),
            meshData.indices.size()
        );

        if (meshData.vertices.empty() || meshData.indices.empty())
        {
            return;
        }

        MeshAsset meshAsset = makeMeshAssetFromPhysics(meshData);
        component->debug().mesh.upload(meshAsset);
        component->debug().built = true;
    }

    void PhysicsSystem::ensureDebugMesh(PlayerControllerComponent* component)
    {
        if (component == nullptr)
        {
            return;
        }

        if (!component->debug().enabled || component->debug().built || !component->isPhysicsInitialized())
        {
            return;
        }

        const physics::BodyMeshData meshData = component->getController().getMeshData(false);

        logger::Log::trace(
            "Player debug mesh build: verts={%zu}, indices={%zu}",
            meshData.vertices.size(),
            meshData.indices.size()
        );

        if (meshData.vertices.empty() || meshData.indices.empty())
        {
            return;
        }

        MeshAsset meshAsset = makeMeshAssetFromPhysics(meshData);
        component->debug().mesh.upload(meshAsset);
        component->debug().built = true;
    }

    void PhysicsSystem::registerRigidBody(RigidBodyComponent* component)
    {
        if (component == nullptr)
        {
            return;
        }

        if (std::find(m_rigidBodies.begin(), m_rigidBodies.end(), component) == m_rigidBodies.end())
        {
            m_rigidBodies.push_back(component);
        }
    }

    void PhysicsSystem::unregisterRigidBody(RigidBodyComponent* component)
    {
        std::erase(m_rigidBodies, component);
    }

    void PhysicsSystem::registerPlayerController(PlayerControllerComponent* component)
    {
        if (component == nullptr)
        {
            return;
        }

        if (std::find(m_playerControllers.begin(), m_playerControllers.end(), component) == m_playerControllers.end())
        {
            m_playerControllers.push_back(component);
        }
    }

    void PhysicsSystem::unregisterPlayerController(PlayerControllerComponent* component)
    {
        std::erase(m_playerControllers, component);
    }
}