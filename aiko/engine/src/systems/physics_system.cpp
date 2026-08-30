#include "physics_system.h"

#include "render_system.h"
#include "system_connector.h"
#include "models/mesh_factory.h"
#include "time/time.h"
#include "asset_system.h"
#include "scene_system.h"

#include "internal/entity_registry_access.h"
#include "internal/physics_runtime_components.h"

namespace aiko
{
    PhysicsSystem::PhysicsSystem() = default;

    void PhysicsSystem::connect(ModuleConnector*, SystemConnector* systemConnector)
    {
        BIND_SYSTEM_REQUIRED(RenderSystem, systemConnector, m_renderSystem);
        BIND_SYSTEM_REQUIRED(AssetSystem, systemConnector, m_assetSystem);
        BIND_SYSTEM_REQUIRED(SceneSystem, systemConnector, m_sceneSystem);
    }

    void PhysicsSystem::init()
    {
        m_physics.init();

        m_debugMaterial.m_shaderId = m_assetSystem->load<ShaderAsset>("model");
        m_debugMaterial.m_baseColor = MAGENTA;
        m_debugMaterial.m_useVertexColor = true;
        m_debugMaterial.m_lit = false;
    }

    void PhysicsSystem::update()
    {
        BaseSystem::update();

        entt::registry& registry = EntityRegistryAccess::get(m_sceneSystem->getScene().registry());

        auto rigidBodies = registry.view<PhysicsRigidBodyRuntime>();
        auto playerControllers = registry.view<PhysicsPlayerControllerRuntime>();

        m_physicsAccumulator += Time::it().getDeltaTime();
        constexpr int kMaxSubSteps = 4;

        int subSteps = 0;
        while (m_physicsAccumulator >= physics::kPhysicsDeltaTime && subSteps < kMaxSubSteps)
        {
            for (auto entity : rigidBodies)
            {
                PhysicsRigidBodyRuntime& runtime = rigidBodies.get<PhysicsRigidBodyRuntime>(entity);
                RigidBodyComponent* body = runtime.component;
                if (body != nullptr)
                {
                    runtime.world = &m_physics;
                    body->ensurePhysicsInitialized(m_physics);
                }
            }

            for (auto entity : playerControllers)
            {
                PlayerControllerComponent* controller = playerControllers.get<PhysicsPlayerControllerRuntime>(entity).component;

                if (controller != nullptr)
                {
                    controller->ensurePhysicsInitialized(m_physics);
                }
            }

            for (auto entity : playerControllers)
            {
                PlayerControllerComponent* controller = playerControllers.get<PhysicsPlayerControllerRuntime>(entity).component;

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

            for (auto entity : rigidBodies)
            {
                RigidBodyComponent* body = rigidBodies.get<PhysicsRigidBodyRuntime>(entity).component;
                if (body != nullptr)
                {
                    body->syncFromPhysics(m_physics);
                }
            }

            m_physicsAccumulator -= physics::kPhysicsDeltaTime;
            ++subSteps;
        }

        for (auto entity : rigidBodies)
        {
            RigidBodyComponent* body = rigidBodies.get<PhysicsRigidBodyRuntime>(entity).component;
            ensureDebugMesh(body);
        }

        for (auto entity : playerControllers)
        {
            PlayerControllerComponent* controller = playerControllers.get<PhysicsPlayerControllerRuntime>(entity).component;
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

        entt::registry& registry = EntityRegistryAccess::get(m_sceneSystem->getScene().registry());

        auto rigidBodies = registry.view<PhysicsRigidBodyRuntime>();
        auto playerControllers = registry.view<PhysicsPlayerControllerRuntime>();

        for (auto entity : rigidBodies)
        {
            RigidBodyComponent* body = rigidBodies.get<PhysicsRigidBodyRuntime>(entity).component;
            if (body != nullptr && body->debug().enabled && body->debug().built)
            {
                Transform debugTransform = body->getWorldTransform();
                debugTransform.scale *= 1.01f;
                m_renderSystem->render(debugTransform, body->debug().mesh, m_debugMaterial);
            }
        }

        for (auto entity : playerControllers)
        {
            PlayerControllerComponent* controller = playerControllers.get<PhysicsPlayerControllerRuntime>(entity).component;
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
        entt::registry& registry = EntityRegistryAccess::get(m_sceneSystem->getScene().registry());

        auto rigidBodies = registry.view<PhysicsRigidBodyRuntime>();

        for (auto entity : rigidBodies)
        {
            PhysicsRigidBodyRuntime& runtime = rigidBodies.get<PhysicsRigidBodyRuntime>(entity);
            if (runtime.component != nullptr)
            {
                runtime.component->physicsShutdown(m_physics);
            }
            runtime.world = nullptr;
        }

        auto playerControllers = registry.view<PhysicsPlayerControllerRuntime>();

        for (auto entity : playerControllers)
        {
            PlayerControllerComponent* controller = playerControllers.get<PhysicsPlayerControllerRuntime>(entity).component;
            if (controller != nullptr)
            {
                controller->physicsShutdown();
            }
        }

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

}