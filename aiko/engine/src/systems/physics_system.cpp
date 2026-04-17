#include "physics_system.h"

#include "render_system.h"
#include "system_connector.h"
#include "models/mesh_factory.h"
#include "time/time.h"

namespace aiko
{

    PhysicsSystem::PhysicsSystem()
    {

    }

    void PhysicsSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        BIND_SYSTEM_REQUIRED(RenderSystem, systemConnector, m_renderSystem);
    }

    void PhysicsSystem::init()
    {
        m_physics.init();

        generatedDebugPhysicsScene();

        m_debugMaterial.m_shaderId = aiko->getComponentAssetAccess()->registerShader("model");
        m_debugMaterial.m_baseColor = WHITE;
        m_debugMaterial.m_useVertexColor = true;
        m_debugMaterial.m_lit = true;

    }

    void PhysicsSystem::update()
    {
        BaseSystem::update();

        m_physicsAccumulator += Time::it().getDeltaTime();
        constexpr int kMaxSubSteps = 4;

        int subSteps = 0;
        while (m_physicsAccumulator >= physics::kPhysicsDeltaTime && subSteps < kMaxSubSteps)
        {
            const physics::StepDesc stepDesc
            {
                .deltaTime = physics::kPhysicsDeltaTime,
                .collisionSteps = 1,
            };

            m_physics.step(stepDesc);

            m_physicsAccumulator -= physics::kPhysicsDeltaTime;
            ++subSteps;
        }

        for (DebugBody& body : bodies)
        {
            if (body.body != physics::InvalidBodyId)
            {
                body.transform = m_physics.getTransform(body.body);
            }
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
        for (DebugBody& body : bodies)
        {
            if (body.body != physics::InvalidBodyId)
            {
                m_renderSystem->render(body.transform, body.mesh, m_debugMaterial);
            }
        }
    }

    void PhysicsSystem::dispose()
    {
        for (DebugBody& body : bodies)
        {
            if (body.body != physics::InvalidBodyId)
            {
                m_physics.destroyBody(body.body);
                body.body = physics::InvalidBodyId;
            }
        }
        bodies.clear();
        m_physics.shutdown();
        BaseSystem::dispose();
    }

    void PhysicsSystem::generatedDebugPhysicsScene()
    {
        bodies.clear();

        // generate sphere
        if constexpr (true)
        {

            physics::BodyDesc bodyDesc{};
            bodyDesc.motionType = physics::MotionType::Dynamic;
            bodyDesc.layer = physics::ObjectLayer::Moving;
            bodyDesc.activate = true;

            bodyDesc.transform.position = vec3(0.0f, 64.0f, 0.0f);
            bodyDesc.transform.rotation = vec3(0.0f);
            bodyDesc.transform.scale = vec3(1.0f);

            bodyDesc.shape.type = physics::ShapeType::Sphere;
            bodyDesc.shape.sphere.radius = 1.0f;

            bodyDesc.restitution = 0.85f;

            const physics::BodyId body = m_physics.createBody(bodyDesc);

            AIKO_ASSERT(body != physics::InvalidBodyId, "Invalid body creation")

            DebugBody debugBody =
            {
                .transform = bodyDesc.transform,
                .body = body,
                .mesh = {}
            };

            const physics::BodyMeshData meshData = m_physics.getBodyMeshData(body, false);
            MeshAsset meshAsset = makeMeshAssetFromPhysics(meshData);
            debugBody.mesh.upload(meshAsset);

            bodies.push_back(std::move(debugBody));

            // m_physics.setLinearVelocity(body, vec3(3.0f, 0.0f, 0.0f));
            // m_physics.addImpulse(body, vec3(0.0f, 8.0f, 0.0f));

        }

        // generate floor
        if constexpr (false)
        {

            physics::BodyDesc bodyDesc{};
            bodyDesc.motionType = physics::MotionType::Static;
            bodyDesc.layer = physics::ObjectLayer::NonMoving;
            bodyDesc.activate = false;

            bodyDesc.transform.position = vec3(0.0f, -10.0f, 0.0f);
            bodyDesc.transform.rotation = vec3(0.0f);
            bodyDesc.transform.scale = vec3(1.0f);

            bodyDesc.shape.type = physics::ShapeType::Box;
            bodyDesc.shape.box.halfExtent = vec3(10.0f, 0.25f, 10.0f);

            bodyDesc.restitution = 0.85f;

            const physics::BodyId body = m_physics.createBody(bodyDesc);

            AIKO_ASSERT(body != physics::InvalidBodyId, "Invalid body creation")

            DebugBody debugBody =
            {
                .transform = bodyDesc.transform,
                .body = body,
                .mesh = {}
            };

            const physics::BodyMeshData meshData = m_physics.getBodyMeshData(body, false);
            MeshAsset meshAsset = makeMeshAssetFromPhysics(meshData);
            debugBody.mesh.upload(meshAsset);

            bodies.push_back(std::move(debugBody));

        }

    }

    MeshAsset PhysicsSystem::makeMeshAssetFromPhysics(const physics::BodyMeshData& data)
    {
        MeshAsset asset{};

        AIKO_ASSERT( data.vertices.size() <= std::numeric_limits<uint16_t>::max(), "Physics mesh is too large for MeshAsset uint16_t indices" );

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
}
