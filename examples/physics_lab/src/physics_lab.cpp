#include "physics_lab.h"

#include "components/camera_component.h"
#include "components/light_component.h"
#include "components/mesh_component.h"
#include "components/rigid_body_component.h"
#include "layers/contexts/asset_context.h"
#include "layers/contexts/input_context.h"
#include "layers/contexts/scene_context.h"
#include "models/game_object.h"
#include "models/mesh_factory.h"
#include "physics_checker.h"
#include "physics_utils.h"
#include "scene/scene.h"
#include "systems/physics_system.h"
#include "systems/render_system.h"
#include "systems/scene_system.h"
#include "systems/system_connector.h"
#include "types/color.h"

#include <events/events.hpp>

#include <aiko_player_controller.h>
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <imgui.h>
#include <iostream>
#include <limits>
#include <physics_transform.h>
#include <thread>
#include <utility>

namespace aiko::lab
{

    void PhysicsLab::connect(SystemConnector& connector)
    {
        BIND_SYSTEM_REQUIRED_REF(PhysicsSystem, connector, m_physicsSystem);
        BIND_SYSTEM_REQUIRED_REF(SceneSystem, connector, m_sceneSystem);
        BIND_SYSTEM_REQUIRED_REF(RenderSystem, connector, m_renderSystem);
    }

    void PhysicsLab::init()
    {
        m_modelShader = assets().loadShader("model");
        EventSystem::it().bind(this, &PhysicsLab::onContact);
        EventSystem::it().bind(this, &PhysicsLab::observeContact);
        EventSystem::it().bind(this, &PhysicsLab::onContactEnded);
        m_ray.from = {0.0f, 18.0f, 0.0f};
        m_ray.to = {0.0f, -2.0f, 0.0f};
        m_sphereQuery.center = {0.0f, 2.0f, 0.0f};
        m_sphereQuery.radius = 3.0f;
        m_boxQuery.center = {6.0f, 2.0f, 0.0f};
        m_boxQuery.halfExtent = {2.0f, 2.0f, 2.0f};
        m_boxQuery.rotation = {0.0f, 25.0f, 0.0f};
        runChecks(m_checksPassed, m_checksFailed, m_failures);
        buildScene();
    }

    GameObject* PhysicsLab::find(const uuid::Uuid& id) const
    {
        return m_sceneSystem->findGameObject(id);
    }

    RigidBodyComponent* PhysicsLab::selectedBody() const
    {
        if (m_selected < 0 || static_cast<size_t>(m_selected) >= m_bodies.size())
        {
            return nullptr;
        }
        GameObject* object = find(m_bodies[m_selected].objectId);
        return object ? object->getComponent<RigidBodyComponent>() : nullptr;
    }

    PlayerControllerComponent* PhysicsLab::controller() const
    {
        GameObject* object = m_controllerId ? find(*m_controllerId) : nullptr;
        return object ? object->getComponent<PlayerControllerComponent>() : nullptr;
    }

    GameObject* PhysicsLab::addBody(const char* name, physics::BodyDesc desc, Color color, GameObject* parent)
    {
        GameObject* object = parent ? Instantiate(parent, name) : Instantiate(name);
        object->transform().rotation = desc.transform.rotation;
        auto* mesh = object->addComponent<MeshComponent>();
        mesh->load(visualMesh(desc.shape));
        configureMaterial(mesh->getMaterial(), m_modelShader, color);
        desc.debugDraw = m_debug;
        object->addComponent<RigidBodyComponent>()->create(desc);
        m_bodies.push_back(BodyEntry{name, object->uuid(), desc});
        return object;
    }

    void PhysicsLab::buildScene()
    {
        input().setMouseCaptured(false);
        m_camera = nullptr;
        m_controllerId.reset();
        m_platformId.reset();
        m_deleteOnContact.reset();
        m_bodies.clear();
        m_sceneSystem->clear();
        m_contactsStarted = m_contactsEnded = m_missingInObserver = 0;
        m_platformTime = 0.0f;
        m_selected = 5;

        scene().clearColor() = BLACK;
        scene().ambientLight().color = WHITE;
        scene().ambientLight().intensity = 0.3f;

        Instantiate("Light")->addComponent<LightComponent>()->setDirectional(WHITE, vec3{-0.5f, -1.0f, -0.3f}, 1.0f);

        m_camera = scene().createCamera(camera::CameraController::Fly);
        m_camera->getCamera().position = {0.0f, 16.0f, 34.0f};
        m_camera->getCamera().target = {0.0f, 2.0f, 0.0f};

        addBody("Floor", boxDesc({0.0f, -0.5f, 0.0f}, {22.0f, 0.5f, 16.0f}, physics::MotionType::Static), GRAY);

        constexpr float wallHeight = 6.0f;
        constexpr float wallThickness = 0.5f;

        const auto addWall = [this](const char* name, vec3 position, vec3 halfExtent)
        {
            physics::BodyDesc wall = boxDesc( position, halfExtent, physics::MotionType::Static);

            wall.friction = 0.4f;
            wall.restitution = 0.6f;

            addBody(name, wall, Color{0.25f, 0.3f, 0.4f, 0.2f});
        };

        addWall("Left boundary", {-22.0f, wallHeight * 0.5f, 0.0f}, {wallThickness, wallHeight * 0.5f, 16.5f});
        addWall("Right boundary", {22.0f, wallHeight * 0.5f, 0.0f}, {wallThickness, wallHeight * 0.5f, 16.5f});
        addWall("Back boundary", {0.0f, wallHeight * 0.5f, -16.0f}, {22.5f, wallHeight * 0.5f, wallThickness});
        addWall("Front boundary", {0.0f, wallHeight * 0.5f, 16.0f}, {22.5f, wallHeight * 0.5f, wallThickness});

        physics::BodyDesc box = boxDesc({0.0f, 8.0f, 0.0f}, {0.75f, 1.0f, 0.5f});
        box.transform.rotation = {20.0f, 35.0f, 15.0f};
        addBody("Force / recreation box", box, BLUE);

        physics::BodyDesc sphere{};
        sphere.motionType = physics::MotionType::Dynamic;
        sphere.layer = physics::ObjectLayer::Moving;
        sphere.shape.type = physics::ShapeType::Sphere;
        sphere.shape.sphere.radius = 0.75f;
        sphere.transform.position = {-5.0f, 7.0f, 0.0f};
        sphere.restitution = 0.65f;
        addBody("Sphere", sphere, RED);

        physics::BodyDesc capsule = sphere;
        capsule.shape.type = physics::ShapeType::Capsule;
        capsule.shape.capsule.radius = 0.5f;
        capsule.shape.capsule.halfHeight = 0.75f;
        capsule.transform.position = {4.0f, 8.0f, 0.0f};
        capsule.restitution = 0.0f;
        addBody("Capsule", capsule, GREEN);

        GameObject* parent = Instantiate("Rotated parent");
        parent->transform().position = {8.0f, 0.0f, 0.0f};
        parent->transform().rotation = {0.0f, 35.0f, 0.0f};

        physics::BodyDesc child = box;
        child.transform.position = {0.0f, 9.0f, 0.0f};
        addBody("Parented box", child, YELLOW, parent);

        physics::BodyDesc platform = boxDesc({12.0f, 2.0f, 6.0f}, {2.5f, 0.4f, 2.0f}, physics::MotionType::Kinematic);
        m_platformId = addBody("Kinematic platform", platform, BLUE)->uuid();
        addBody("Platform rider", boxDesc({12.0f, 5.0f, 6.0f}, vec3{0.6f}), YELLOW);

        physics::BodyDesc sensor = boxDesc({0.0f, 4.0f, -8.0f}, {1.8f, 1.0f, 1.8f}, physics::MotionType::Static);
        sensor.isSensor = true;
        addBody("Sensor volume", sensor, Color{0.0f, 0.8f, 1.0f, 0.2f});
        sphere.transform.position = {0.0f, 10.0f, -8.0f};
        sphere.restitution = 0.0f;
        addBody("Sensor falling sphere", sphere, RED);

        for (int i = 0; i < 3; ++i)
        {
            sphere.transform.position = {-9.0f + i * 3.0f, 9.0f, -11.0f};
            sphere.restitution = i * 0.45f;
            const std::string name = "Restitution " + std::to_string(i);
            addBody(name.c_str(), sphere, Color{0.3f + i * 0.3f, 0.4f, 0.8f, 1.0f});
        }

        for (int i = 0; i < 5; ++i)
        {
            const std::string name = "Stack " + std::to_string(i);
            addBody(name.c_str(), boxDesc({8.0f, 0.6f + i * 1.15f, -7.0f}, vec3{0.55f}), i % 2 ? BLUE : YELLOW);
        }

        physics::BodyDesc ramp = boxDesc({-12.0f, 1.5f, 5.0f}, {4.0f, 0.25f, 3.0f}, physics::MotionType::Static);
        ramp.transform.rotation.z = 20.0f;
        ramp.friction = 1.0f;
        addBody("Friction ramp", ramp, GRAY);

        physics::BodyDesc slider = boxDesc({-12.0f, 5.0f, 3.8f}, vec3{0.5f});
        slider.friction = 0.0f;
        addBody("Low friction", slider, RED);
        slider.transform.position.z = 6.2f;
        slider.friction = 1.0f;
        addBody("High friction", slider, GREEN);

        physics::BodyDesc meshRamp{};
        meshRamp.shape.type = physics::ShapeType::TriangleMesh;
        meshRamp.transform.position = {-13.0f, 0.0f, -6.0f};
        meshRamp.shape.triangleMesh.vertices = {
            {-3.0f, 0.0f, -2.0f}, {3.0f, 2.0f, -2.0f},
            {3.0f, 2.0f, 2.0f}, {-3.0f, 0.0f, 2.0f}};
        meshRamp.shape.triangleMesh.indices = {0, 2, 1, 0, 3, 2};
        addBody("Triangle mesh ramp", meshRamp, GREEN);
        sphere.transform.position = {-13.0f, 6.0f, -6.0f};
        addBody("Mesh ramp sphere", sphere, RED);

        m_controllerDesc = {};
        m_controllerDesc.physics.position = {0.0f, 3.0f, 0.0f};
        m_controllerDesc.physics.radius = 0.3f;
        m_controllerDesc.physics.halfHeight = 0.6f;
        m_controllerDesc.physics.yawRadians = math::radians(180.0f);
        m_controllerDesc.physics.walkSpeed = 5.0f;
        m_controllerDesc.physics.jumpSpeed = 5.0f;
        m_controllerDesc.eyeHeight = 0.65f;
        m_controllerDesc.pitchRadians = -0.12f;
        m_controllerDesc.debugDraw = m_debug;

        GameObject* playerParent = Instantiate("Controller parent");
        playerParent->transform().position = {-2.0f, 0.0f, 9.0f};
        playerParent->transform().rotation.y = 30.0f;
        GameObject* player = Instantiate(playerParent, "Player controller");
        m_controllerId = player->uuid();
        physics::ShapeDesc playerShape{};
        playerShape.type = physics::ShapeType::Capsule;
        playerShape.capsule.radius = m_controllerDesc.physics.radius;
        playerShape.capsule.halfHeight = m_controllerDesc.physics.halfHeight;

        MeshComponent* playerMesh = player->addComponent<MeshComponent>();
        playerMesh->load(visualMesh(playerShape));
        configureMaterial(playerMesh->getMaterial(), m_modelShader, YELLOW);
        player->addComponent<PlayerControllerComponent>()->create(m_controllerDesc);
    }

    void PhysicsLab::recreate(BodyEntry& entry)
    {
        GameObject* object = find(entry.objectId);
        if (object == nullptr)
        {
            return;
        }

        RigidBodyComponent* body = object->getComponent<RigidBodyComponent>();
        if (body == nullptr)
        {
            return;
        }
        object->transform().rotation = entry.initial.transform.rotation;
        body->create(entry.initial);
        body->debug().enabled = m_debug;
    }

    void PhysicsLab::applyAction()
    {
        const Action action = std::exchange(m_action, Action::None);
        if (action == Action::None)
        {
            return;
        }
        if (action == Action::ResetScene)
        {
            buildScene();
            return;
        }
        if (action == Action::RunChecks)
        {
            runChecks(m_checksPassed, m_checksFailed, m_failures);
            return;
        }
        if (action == Action::Stall)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            return;
        }
        if (action == Action::RecreateController || action == Action::ToggleController)
        {
            if (auto* player = controller())
            {
                if (action == Action::RecreateController)
                {
                    player->create(m_controllerDesc);
                }
                else
                {
                    player->setEnabled(!player->isEnabled());
                }
            }
            return;
        }
        RigidBodyComponent* body = selectedBody();
        if (body == nullptr)
        {
            return;
        }
        BodyEntry& entry = m_bodies[m_selected];
        if (action == Action::Recreate)
        {
            recreate(entry);
            return;
        }
        if (action == Action::DeleteOnContact)
        {
            m_deleteOnContact = entry.objectId;
            body->setEnabled(true);
            recreate(entry);
            return;
        }
        if (action == Action::ToggleEnabled)
        {
            body->setEnabled(!body->isEnabled());
            return;
        }
        if (action == Action::Remove)
        {
            m_sceneSystem->destroyGameObject(body->getGameObject());
            return;
        }
        if (body->isPhysicsInitialized() == false)
        {
            return;
        }

        physics::AikoPhysics& world = m_physicsSystem->getPhysics();
        const physics::BodyId id = body->getBodyId();
        const physics::MotionType motion = world.getMotionType(id);
        const bool dynamic = motion == physics::MotionType::Dynamic;
        switch (action)
        {
            case Action::Impulse:
                if (dynamic)
                {
                    world.addImpulse(id, m_vector);
                }
                break;
            case Action::Force:
                if (dynamic)
                {
                    world.addForce(id, m_vector);
                }
                break;
            case Action::Torque:
                if (dynamic)
                {
                    world.addTorque(id, m_vector);
                }
                break;
            case Action::AngularImpulse:
                if (dynamic)
                {
                    world.addAngularImpulse(id, m_vector);
                }
                break;
            case Action::LinearVelocity:
                if (motion != physics::MotionType::Static)
                {
                    world.setLinearVelocity(id, m_vector);
                }
                break;
            case Action::AngularVelocity:
                if (motion != physics::MotionType::Static)
                {
                    world.setAngularVelocity(id, m_vector);
                }
                break;
            case Action::Stop:
                if (motion != physics::MotionType::Static)
                {
                    world.setLinearVelocity(id, vec3{0.0f});
                    world.setAngularVelocity(id, vec3{0.0f});
                }
                break;
            case Action::Teleport:
            {
                Transform pose = world.getTransform(id);
                pose.position += vec3{0.0f, 5.0f, 0.0f};
                world.setTransform(id, pose);
                body->syncFromPhysics(world);
                break;
            }
            case Action::ToggleSensor:
                world.setIsSensor(id, !world.isSensor(id));
                break;
            case Action::Gravity:
                if (dynamic)
                {
                    world.setGravityFactor(id, m_gravityFactor); break;
                }
                break;
            case Action::Static:
            case Action::Kinematic:
            case Action::Dynamic:
            {
                const physics::MotionType next = action == Action::Static ? physics::MotionType::Static : action == Action::Kinematic ? physics::MotionType::Kinematic : physics::MotionType::Dynamic;
                if (!world.setMotionType(id, next))
                {
                    logger::Log::warning("Physics Lab: selected body does not support that motion type");
                }

                break;
            }
            default: break;
        }
    }

    void PhysicsLab::updateController()
    {
        PlayerControllerComponent* player = controller();
        if (player == nullptr || m_camera == nullptr)
        {
            input().setMouseCaptured(false);
            return;
        }
        const bool controlling = m_followController && player->isActiveAndEnabled();
        const auto& io = ImGui::GetIO();
        vec2 move{0.0f};
        if (controlling == true && io.WantCaptureKeyboard == false)
        {
            move.x = (input().isKeyPressed(Key::KEY_D) ? 1.0f : 0.0f) - (input().isKeyPressed(Key::KEY_A) ? 1.0f : 0.0f);
            move.y = (input().isKeyPressed(Key::KEY_W) ? 1.0f : 0.0f) - (input().isKeyPressed(Key::KEY_S) ? 1.0f : 0.0f);
            player->setJumpPressed(input().isKeyJustPressed(Key::KEY_SPACE));
        }
        player->setMoveInput(move);

        const bool looking = controlling && !io.WantCaptureMouse && ImGui::IsMouseDown(ImGuiMouseButton_Right);
        input().setMouseCaptured(looking);
        if (looking == true)
        {
            const vec2 delta = input().getMouseDelta();
            player->addLookDelta(delta.x * 0.002f, delta.y * 0.002f);
        }
        m_camera->setCameraController(controlling ? camera::CameraController::Static : camera::CameraController::Fly);
        if (controlling == true)
        {
            Camera& camera = m_camera->getCamera();
            camera.position = m_thirdPerson ? player->getThirdPersonCameraPosition() : player->getCameraPosition();
            camera.target = m_thirdPerson ? player->getCameraLookTarget() : camera.position + player->getCameraForward();
        }
        if (GameObject* object = m_controllerId ? find(*m_controllerId) : nullptr)
        {
            if (MeshComponent* mesh = object->getComponent<MeshComponent>())
            {
                mesh->setEnabled(controlling == false || m_thirdPerson);
            }
        }
    }

    void PhysicsLab::updateQueries()
    {
        physics::AikoPhysics& world = m_physicsSystem->getPhysics();
        m_rayHit = {};
        m_rayHasHit = world.castRay(m_ray, m_rayHit);
        m_sphereHits = world.overlapSphere(m_sphereQuery);
        m_boxHits = world.overlapBox(m_boxQuery);
    }

    void PhysicsLab::update()
    {
        applyAction();
        for (const auto& entry : m_bodies)
        {
            if (GameObject* object = find(entry.objectId))
            {
                if (RigidBodyComponent* body = object->getComponent<RigidBodyComponent>())
                {
                    body->debug().enabled = m_debug;
                }
            }
        }

        if (PlayerControllerComponent* player = controller())
        {
            player->debug().enabled = m_debug;
        }

        if (m_animatePlatform && m_platformId)
        {
            GameObject* object = find(*m_platformId);
            RigidBodyComponent* body = object ? object->getComponent<RigidBodyComponent>() : nullptr;
            if (body != nullptr && body->isPhysicsInitialized())
            {
                physics::AikoPhysics& world = m_physicsSystem->getPhysics();
                const physics::BodyId id = body->getBodyId();
                if (world.getMotionType(id) == physics::MotionType::Kinematic)
                {
                    const float dt = std::max(getDeltaTime(), 0.0001f);
                    m_platformTime += dt;
                    Transform target = world.getTransform(id);
                    target.position = {12.0f + 3.0f * std::sin(m_platformTime), 2.0f, 6.0f};
                    world.moveKinematic(id, target, dt);
                }
            }
        }
        else if (m_platformId)
        {
            GameObject* object = find(*m_platformId);
            RigidBodyComponent* body = object ? object->getComponent<RigidBodyComponent>() : nullptr;
            if (body != nullptr && body->isPhysicsInitialized())
            {
                physics::AikoPhysics& world = m_physicsSystem->getPhysics();
                if (world.getMotionType(body->getBodyId()) == physics::MotionType::Kinematic)
                {
                    world.setLinearVelocity(body->getBodyId(), vec3{0.0f});
                }
            }
        }
        updateController();
        updateQueries();
    }

    void PhysicsLab::onContact(const PhysicsContactStartedEvent& event)
    {
        ++m_contactsStarted;
        if (m_deleteOnContact == std::nullopt)
        {
            return;
        }
        if (event.a.objectId == *m_deleteOnContact || event.b.objectId == *m_deleteOnContact)
        {
            GameObject* object = find(*m_deleteOnContact);
            m_deleteOnContact.reset();
            if (object != nullptr)
            {
                m_sceneSystem->destroyGameObject(object);
            }
        }
    }

    void PhysicsLab::observeContact(const PhysicsContactStartedEvent& event)
    {
        if (find(event.a.objectId) == nullptr || find(event.b.objectId) == nullptr)
        {
            ++m_missingInObserver;
        }
    }

    void PhysicsLab::onContactEnded(const PhysicsContactEndedEvent&)
    {
        ++m_contactsEnded;
    }

    void PhysicsLab::render()
    {
        drawQueries();
        drawPanel();
    }

    void PhysicsLab::drawPanel()
    {
        ImGui::SetNextWindowSize(ImVec2(420.0f, 700.0f), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Physics Lab") == false)
        {
            ImGui::End();
            return;
        }
        const auto button = [this](const char* text, Action action)
        {
            if (ImGui::Button(text)) m_action = action;
        };
        button("Reset scene", Action::ResetScene);
        ImGui::SameLine(); button("Run regression checks", Action::RunChecks);
        ImGui::Text("Checks: %d passed / %d failed", m_checksPassed, m_checksFailed);
        for (const auto& failure : m_failures)
        {
            ImGui::TextWrapped("FAIL: %s", failure.c_str());
        }
        ImGui::Checkbox("Collider overlays", &m_debug);
        ImGui::Checkbox("Animate kinematic platform", &m_animatePlatform);
        button("Simulate a 250 ms stall", Action::Stall);
        ImGui::TextWrapped("Collider dimensions are explicit. Object scale does not resize a collider.");

        if (ImGui::CollapsingHeader("Body controls", ImGuiTreeNodeFlags_DefaultOpen))
        {
            const char* name = m_bodies.empty() ? "None" : m_bodies[m_selected].name.c_str();
            if (ImGui::BeginCombo("Body", name))
            {
                for (int i = 0; i < static_cast<int>(m_bodies.size()); ++i)
                {
                    if (ImGui::Selectable(m_bodies[i].name.c_str(), m_selected == i))
                    {
                        m_selected = i;
                    }
                }
                ImGui::EndCombo();
            }
            RigidBodyComponent* body = selectedBody();
            if (body == nullptr)
            {
                ImGui::TextUnformatted("Object removed. Reset the scene to restore it.");
            }
            else
            {
                button("Recreate at spawn", Action::Recreate);
                ImGui::SameLine();
                button("Enable / disable", Action::ToggleEnabled);
                button("Remove", Action::Remove);
                ImGui::SameLine();
                button("Recreate, delete on contact", Action::DeleteOnContact);
                if (body->isPhysicsInitialized())
                {
                    physics::AikoPhysics& world = m_physicsSystem->getPhysics();
                    const physics::BodyId id = body->getBodyId();
                    const physics::MotionType motion = world.getMotionType(id);
                    const vec3 velocity = world.getLinearVelocity(id);
                    const vec3 angular = world.getAngularVelocity(id);
                    const Transform pose = world.getTransform(id);
                    ImGui::Text("Motion: %s | sensor: %s", motion == physics::MotionType::Dynamic ? "Dynamic" : motion == physics::MotionType::Kinematic ? "Kinematic" : "Static", world.isSensor(id) ? "yes" : "no");
                    ImGui::Text("Position: %.2f %.2f %.2f", pose.position.x, pose.position.y, pose.position.z);
                    ImGui::Text("Velocity: %.2f %.2f %.2f", velocity.x, velocity.y, velocity.z);
                    ImGui::Text("Angular: %.2f %.2f %.2f", angular.x, angular.y, angular.z);
                    ImGui::InputFloat3("Vector", &m_vector.x);
                    button("Impulse", Action::Impulse);
                    ImGui::SameLine();
                    button("Force once", Action::Force);
                    button("Angular impulse", Action::AngularImpulse);
                    ImGui::SameLine();
                    button("Torque once", Action::Torque);
                    button("Set velocity", Action::LinearVelocity);
                    ImGui::SameLine();
                    button("Set angular velocity", Action::AngularVelocity);
                    button("Stop velocities", Action::Stop);
                    ImGui::SameLine();
                    button("Teleport up", Action::Teleport);
                    ImGui::SliderFloat("Gravity factor", &m_gravityFactor, 0.0f, 3.0f);
                    button("Apply gravity factor", Action::Gravity);
                    button("Toggle sensor", Action::ToggleSensor);
                    if (m_bodies[m_selected].initial.shape.type != physics::ShapeType::TriangleMesh)
                    {
                        button("Static", Action::Static);
                        ImGui::SameLine();
                        button("Kinematic", Action::Kinematic);
                        ImGui::SameLine();
                        button("Dynamic", Action::Dynamic);
                    }
                    physics::BodyDesc& desc = m_bodies[m_selected].initial;
                    ImGui::SliderFloat("Spawn friction", &desc.friction, 0.0f, 1.0f);
                    ImGui::SliderFloat("Spawn restitution", &desc.restitution, 0.0f, 1.0f);
                    ImGui::SliderFloat("Spawn mass", &desc.mass, 0.1f, 20.0f);
                    ImGui::TextUnformatted("Spawn settings apply when recreating.");
                }
            }
        }
        if (ImGui::CollapsingHeader("Controller"))
        {
            ImGui::Checkbox("Control player / follow camera", &m_followController);
            ImGui::Checkbox("Third-person camera", &m_thirdPerson);
            ImGui::TextWrapped("WASD moves, Space jumps. Hold right mouse to look. Disable player control to use the fly camera.");
            button("Recreate controller", Action::RecreateController);
            ImGui::SameLine();
            button("Enable / disable controller", Action::ToggleController);
            ImGui::SliderFloat("Walk speed", &m_controllerDesc.physics.walkSpeed, 0.5f, 12.0f);
            ImGui::SliderFloat("Jump speed", &m_controllerDesc.physics.jumpSpeed, 1.0f, 10.0f);
            ImGui::SliderFloat("Max slope", &m_controllerDesc.physics.maxSlopeAngleDegrees, 5.0f, 75.0f);
            ImGui::TextUnformatted("Settings apply on controller recreation.");
            if (PlayerControllerComponent* player = controller())
            {
                ImGui::Text("Initialized: %s | grounded: %s", player->isPhysicsInitialized() ? "yes" : "no", player->getController().isOnGround() ? "yes" : "no");
                ImGui::Text("Yaw / pitch: %.2f / %.2f", player->getYaw(), player->getPitch());
            }
        }
        if (ImGui::CollapsingHeader("Queries"))
        {
            ImGui::Checkbox("Draw query volumes", &m_showQueries);
            ImGui::InputFloat3("Ray from", &m_ray.from.x);
            ImGui::InputFloat3("Ray to", &m_ray.to.x);
            ImGui::Text("Ray hit: %s", m_rayHasHit ? "yes" : "no");
            if (m_rayHasHit)
            {
                ImGui::Text("Fraction: %.3f | body: %u", m_rayHit.fraction, unsigned(m_rayHit.body));
            }
            ImGui::InputFloat3("Sphere center", &m_sphereQuery.center.x);
            ImGui::SliderFloat("Sphere radius", &m_sphereQuery.radius, 0.1f, 8.0f);
            ImGui::Text("Sphere hits: %zu", m_sphereHits.size());
            ImGui::InputFloat3("Box center", &m_boxQuery.center.x);
            ImGui::SliderFloat3("Box half extent", &m_boxQuery.halfExtent.x, 0.1f, 8.0f);
            ImGui::SliderFloat3("Box rotation", &m_boxQuery.rotation.x, -180.0f, 180.0f);
            ImGui::Text("Box hits: %zu", m_boxHits.size());
        }
        ImGui::Separator();
        ImGui::Text("Contacts started / ended: %llu / %llu", static_cast<unsigned long long>(m_contactsStarted), static_cast<unsigned long long>(m_contactsEnded));
        ImGui::Text("Second listener saw removed participant: %llu", static_cast<unsigned long long>(m_missingInObserver));
        ImGui::TextUnformatted("Invalid inputs that intentionally assert require a separate process.");
        ImGui::End();
    }

    void PhysicsLab::drawQueries()
    {
        if (!m_showQueries)
        {
            return;
        }
        m_renderSystem->renderLine(m_ray.from, m_rayHasHit ? m_rayHit.position : m_ray.to, m_rayHasHit ? GREEN : RED);
        if (m_rayHasHit)
        {
            m_renderSystem->renderLine(m_ray.from, m_rayHit.position, GREEN);
            m_renderSystem->renderLine(m_rayHit.position, m_ray.to, Color{0.4f, 0.4f, 0.4f, 1.0f});
            m_renderSystem->renderSphere(m_rayHit.position, vec3{0.15f}, YELLOW, 12);
            m_renderSystem->renderLine(m_rayHit.position, m_rayHit.position + m_rayHit.normal, YELLOW);
        }
        else
        {
            m_renderSystem->renderLine(m_ray.from, m_ray.to, RED);
        }
        m_renderSystem->renderSphere(m_sphereQuery.center, vec3{2.0f * m_sphereQuery.radius}, Color{0.2f, 0.5f, 1.0f, 0.12f}, 16);

        Transform box{};
        box.position = m_boxQuery.center;
        box.rotation = m_boxQuery.rotation;
        box.scale = m_boxQuery.halfExtent * 2.0f;
        const mat4 matrix = box.getWorldMatrix();
        std::array<vec3, 8> corners{};
        for (int i = 0; i < 8; ++i)
        {
            corners[i] = math::transformPoint(matrix, vec3{(i & 1) ? 0.5f : -0.5f, (i & 2) ? 0.5f : -0.5f, (i & 4) ? 0.5f : -0.5f});
        }

        for (int i = 0; i < 8; ++i)
        {
            for (int bit : {1, 2, 4})
            {
                if ((i & bit) == 0)
                {
                    m_renderSystem->renderLine(corners[i], corners[i | bit], YELLOW);
                }
            }
        }
    }

    void PhysicsLab::dispose()
    {
        EventSystem::it().unbindAll(this);
        input().setMouseCaptured(false);
        m_camera = nullptr;
        m_controllerId.reset();
        m_platformId.reset();
        m_deleteOnContact.reset();
        m_bodies.clear();
    }
}
