#include "physics_checker.h"

#include "physics_utils.h"
#include "scene/scene.h"
#include "components/rigid_body_component.h"
#include "components/player_controller_component.h"

#include <physics_transform.h>

namespace aiko::lab
{

    bool near(const vec3& a, const vec3& b, float tolerance)
    {
        return math::length(a - b) < tolerance;
    }

    bool samePose(const Transform& a, const Transform& b)
    {
        const std::array<vec3, 4> probes{
            vec3{0.0f},
            vec3{1.0f, 0.0f, 0.0f},
            vec3{0.0f, 1.0f, 0.0f},
            vec3{0.0f, 0.0f, 1.0f}
        };

        for (const vec3& probe : probes)
        {
            if (!near(math::transformPoint(a.getWorldMatrix(), probe), math::transformPoint(b.getWorldMatrix(), probe)))
            {
                return false;
            }
        }
        return true;
    }

    bool contains(const std::vector<physics::BodyId>& ids, physics::BodyId id)
    {
        return std::find(ids.begin(), ids.end(), id) != ids.end();
    }

    bool check(bool condition, const std::string& name, int& checksPassed, int& checksFailed, std::vector<std::string>& failures)
    {
        if (condition)
        {
            ++checksPassed;
            logger::Log::infoFmt("[Physics Lab] PASS: {}", name);
        }
        else
        {
            ++checksFailed;
            failures.push_back(name);
            logger::Log::errorFmt("[Physics Lab] FAIL: {}", name);
        }
        return condition;
    }

    void runChecks(int& checksPassed, int& checksFailed, std::vector<std::string>& failures)
    {
        checksPassed = 0;
        checksFailed = 0;
        failures.clear();

        Transform parent{};
        parent.position = {10.0f, 2.0f, -3.0f};
        parent.rotation = {20.0f, 35.0f, -15.0f};
        parent.scale = vec3{2.0f};
        Transform child{};
        child.position = {2.0f, 3.0f, -1.0f};
        child.rotation = {-10.0f, 25.0f, 40.0f};
        child.setParent(&parent);
        Transform expected{};
        const Transform pose = physics::toPhysicsTransform(child);
        expected.position = pose.position;
        expected.rotation = pose.rotation;
        expected.scale = vec3{2.0f};
        check(samePose(child, expected), "parented pose conversion", checksPassed, checksFailed, failures);
        child.position = vec3{0.0f};
        child.rotation = vec3{0.0f};
        check(physics::applyPhysicsTransform(child, pose), "parented pose application", checksPassed, checksFailed, failures);
        check(samePose(child, expected), "parented pose round trip", checksPassed, checksFailed, failures);
        parent.scale.x = 0.0f;
        const vec3 oldPosition = child.position, oldRotation = child.rotation, oldScale = child.scale;
        check(!physics::applyPhysicsTransform(child, pose), "reject singular parent", checksPassed, checksFailed, failures);
        check(near(child.position, oldPosition) && near(child.rotation, oldRotation) && near(child.scale, oldScale) && child.getParent() == &parent, "failed conversion is atomic", checksPassed, checksFailed, failures);
        child.clearParent();
        Transform invalidPose{};
        invalidPose.position.x = std::numeric_limits<float>::quiet_NaN();
        check(!physics::applyPhysicsTransform(child, invalidPose), "reject non-finite pose", checksPassed, checksFailed, failures);

        physics::AikoPhysics world;
        physics::BodyDesc desc = boxDesc({0.0f, 10.0f, 0.0f}, vec3{0.5f});
        check(world.createBody(desc) == physics::InvalidBodyId, "uninitialized world rejects body creation", checksPassed, checksFailed, failures);
        world.init();
        world.init();

        physics::BodyId id = world.createBody(desc);
        if (check(id != physics::InvalidBodyId, "create dynamic box", checksPassed, checksFailed, failures))
        {
            for (const vec3& angles : {vec3{20.0f, 35.0f, 15.0f}, vec3{-40.0f, 89.0f, 70.0f}, vec3{15.0f, -89.0f, -30.0f}})
            {
                Transform target{};
                target.position = desc.transform.position;
                target.rotation = angles;
                world.setTransform(id, target);
                check(samePose(target, world.getTransform(id)), "mixed-axis physics rotation round trip", checksPassed, checksFailed, failures);
            }
            world.setGravityFactor(id, 0.0f);
            check(std::abs(world.getGravityFactor(id)) < 0.001f, "gravity factor getter / setter", checksPassed, checksFailed, failures);
            world.setLinearVelocity(id, {1.0f, 2.0f, 3.0f});
            check(near(world.getLinearVelocity(id), {1.0f, 2.0f, 3.0f}), "linear velocity getter / setter", checksPassed, checksFailed, failures);
            world.setAngularVelocity(id, {0.0f, 1.0f, 0.0f});
            check(near(world.getAngularVelocity(id), {0.0f, 1.0f, 0.0f}), "angular velocity getter / setter", checksPassed, checksFailed, failures);
            world.setLinearVelocity(id, vec3{0.0f});
            world.addImpulse(id, {0.0f, 2.0f, 0.0f});
            check(world.getLinearVelocity(id).y > 0.0f, "linear impulse", checksPassed, checksFailed, failures);
            world.setAngularVelocity(id, vec3{0.0f});
            world.addAngularImpulse(id, {0.0f, 1.0f, 0.0f});
            check(world.getAngularVelocity(id).y > 0.0f, "angular impulse", checksPassed, checksFailed, failures);
            world.setLinearVelocity(id, vec3{0.0f});
            world.setAngularVelocity(id, vec3{0.0f});
            world.addForce(id, {0.0f, 60.0f, 0.0f});
            world.addTorque(id, {0.0f, 1.0f, 0.0f});
            world.step();
            check(world.getLinearVelocity(id).y > 0.0f, "force integration", checksPassed, checksFailed, failures);
            check(world.getAngularVelocity(id).y > 0.0f, "torque integration", checksPassed, checksFailed, failures);
            world.setIsSensor(id, true);
            check(world.isSensor(id), "enable sensor", checksPassed, checksFailed, failures);
            world.setIsSensor(id, false);
            check(!world.isSensor(id), "disable sensor", checksPassed, checksFailed, failures);
            world.setMotionType(id, physics::MotionType::Kinematic);
            check(world.getMotionType(id) == physics::MotionType::Kinematic, "switch to kinematic", checksPassed, checksFailed, failures);
            Transform target{};
            target.position = {4.0f, 10.0f, 0.0f};
            target.rotation = {10.0f, 25.0f, 30.0f};
            world.moveKinematic(id, target, physics::kPhysicsDeltaTime);
            world.step();
            check(samePose(world.getTransform(id), target), "kinematic target over one fixed step", checksPassed, checksFailed, failures);
            world.setMotionType(id, physics::MotionType::Static);
            check(world.getMotionType(id) == physics::MotionType::Static, "switch to static", checksPassed, checksFailed, failures);
            world.setMotionType(id, physics::MotionType::Dynamic);
            check(world.getMotionType(id) == physics::MotionType::Dynamic, "switch back to dynamic", checksPassed, checksFailed, failures);
            world.setLinearVelocity(id, vec3{0.0f}); world.setAngularVelocity(id, vec3{0.0f});
            world.setTransform(id, target);
            physics::RayCastDesc ray{};
            ray.from = {4.0f, 15.0f, 0.0f};
            ray.to = {4.0f, 5.0f, 0.0f};
            physics::RayCastHit hit{};
            check(world.castRay(ray, hit) && hit.body == id && hit.fraction >= 0.0f && hit.fraction <= 1.0f, "ray cast identity and fraction", checksPassed, checksFailed, failures);
            physics::SphereOverlapDesc sphere{};
            sphere.center = target.position;
            sphere.radius = 2.0f;
            check(contains(world.overlapSphere(sphere), id), "sphere overlap", checksPassed, checksFailed, failures);
            physics::BoxOverlapDesc box{};
            box.center = target.position;
            box.halfExtent = vec3{2.0f};
            box.rotation.y = 35.0f;
            check(contains(world.overlapBox(box), id), "rotated box overlap", checksPassed, checksFailed, failures);
            const physics::BodyMeshData local = world.getBodyMeshData(id, false);
            const physics::BodyMeshData global = world.getBodyMeshData(id, true);
            bool meshMatches = !local.vertices.empty() && local.vertices.size() == global.vertices.size();
            for (size_t i = 0; meshMatches && i < local.vertices.size(); ++i)
            {
                meshMatches = near(math::transformPoint(target.getWorldMatrix(), local.vertices[i]), global.vertices[i]);
            }
            check(meshMatches && !local.indices.empty(), "local / world shape mesh extraction", checksPassed, checksFailed, failures);
            world.destroyBody(id);
            check(!contains(world.overlapSphere(sphere), id), "destroy removes body from queries", checksPassed, checksFailed, failures);
        }

        for (float invalid : {0.0f, -1.0f, std::numeric_limits<float>::infinity(), std::numeric_limits<float>::quiet_NaN()})
        {
            physics::BodyDesc bad = desc;
            bad.shape.type = physics::ShapeType::Sphere;
            bad.shape.sphere.radius = invalid;
            physics::BodyId reject = world.createBody(bad);
            check(reject == physics::InvalidBodyId, "reject invalid sphere radius", checksPassed, checksFailed, failures);
            if (reject != physics::InvalidBodyId)
            {
                world.destroyBody(reject);
            }
            bad.shape.type = physics::ShapeType::Box;
            bad.shape.box.halfExtent.x = invalid;
            reject = world.createBody(bad);
            check(reject == physics::InvalidBodyId, "reject invalid box extent", checksPassed, checksFailed, failures);
            if (reject != physics::InvalidBodyId)
            {
                world.destroyBody(reject);
            }
            bad.shape.type = physics::ShapeType::Capsule;
            bad.shape.capsule.halfHeight = invalid;
            reject = world.createBody(bad);
            check(reject == physics::InvalidBodyId, "reject invalid capsule height", checksPassed, checksFailed, failures);
            if (reject != physics::InvalidBodyId)
            {
                world.destroyBody(reject);
            }
            bad.shape.capsule.halfHeight = 0.5f;
            bad.shape.capsule.radius = invalid;
            reject = world.createBody(bad);
            check(reject == physics::InvalidBodyId, "reject invalid capsule radius", checksPassed, checksFailed, failures);
            if (reject != physics::InvalidBodyId)
            {
                world.destroyBody(reject);
            }
            physics::PlayerController rejectedController;
            physics::PlayerControllerDesc badController{};
            badController.radius = invalid;
            check(!rejectedController.init(world, badController), "reject invalid controller radius", checksPassed, checksFailed, failures);
            badController.radius = 0.3f;
            badController.halfHeight = invalid;
            check(!rejectedController.init(world, badController), "reject invalid controller height", checksPassed, checksFailed, failures);
        }
        physics::BodyDesc triangle{};
        triangle.shape.type = physics::ShapeType::TriangleMesh;
        physics::BodyId rejected = world.createBody(triangle);
        check(rejected == physics::InvalidBodyId, "reject empty triangle mesh", checksPassed, checksFailed, failures);
        if (rejected != physics::InvalidBodyId)
        {
            world.destroyBody(rejected);
        }
        triangle.shape.triangleMesh.vertices = {{-1.0f, 0.0f, -1.0f}, {1.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 1.0f}};
        triangle.shape.triangleMesh.indices = {0, 2, 9};
        rejected = world.createBody(triangle);
        check(rejected == physics::InvalidBodyId, "reject triangle index out of range", checksPassed, checksFailed, failures);
        if (rejected != physics::InvalidBodyId)
        {
            world.destroyBody(rejected);
        }
        triangle.shape.triangleMesh.indices = {0, 2, 1};
        triangle.shape.triangleMesh.vertices[0].x = std::numeric_limits<float>::quiet_NaN();
        rejected = world.createBody(triangle);
        check(rejected == physics::InvalidBodyId, "reject non-finite mesh vertex", checksPassed, checksFailed, failures);
        if (rejected != physics::InvalidBodyId)
        {
            world.destroyBody(rejected);
        }

        {
            Scene scene;
            GameObject* object = scene.create("Recreation check");
            object->transform().scale = {2.0f, 0.5f, 1.0f};

            RigidBodyComponent* body = object->addComponent<RigidBodyComponent>();

            physics::BodyDesc spawn = boxDesc({20.0f, 5.0f, 0.0f}, vec3{0.5f});

            spawn.transform.rotation = {20.0f, 35.0f, 15.0f};

            body->create(spawn);
            body->ensurePhysicsInitialized(world);

            const physics::BodyId oldId = body->getBodyId();

            check(body->isPhysicsInitialized(), "component initializes body", checksPassed, checksFailed, failures);
            check(samePose(spawn.transform, world.getTransform(oldId)), "body creation applies descriptor position and rotation", checksPassed, checksFailed, failures);

            body->syncFromPhysics(world);

            check(near(object->transform().scale, {2.0f, 0.5f, 1.0f}), "synchronization preserves visual scale", checksPassed, checksFailed, failures);

            spawn.transform.position.x = 25.0f;
            spawn.transform.rotation = {-15.0f, 60.0f, 10.0f};

            body->create(spawn);
            body->ensurePhysicsInitialized(world);

            const physics::BodyId newId = body->getBodyId();

            check(body->isPhysicsInitialized() && newId != oldId, "recreation replaces body identity", checksPassed, checksFailed, failures);
            check(samePose(spawn.transform, world.getTransform(newId)), "body recreation applies new descriptor rotation", checksPassed, checksFailed, failures);

            physics::SphereOverlapDesc query{};
            query.center = {20.0f, 5.0f, 0.0f};
            query.radius = 0.2f;

            check(!contains(world.overlapSphere(query), oldId), "recreation removes original collider", checksPassed, checksFailed, failures);

            query.center = spawn.transform.position;
            scene.remove(object);

            check(!contains(world.overlapSphere(query), newId), "scene removal releases collider", checksPassed, checksFailed, failures);
        }

        const physics::BodyId floorId = world.createBody(boxDesc({0.0f, -0.5f, 0.0f}, {8.0f, 0.5f, 8.0f}, physics::MotionType::Static));
        if (check(floorId != physics::InvalidBodyId, "controller test floor", checksPassed, checksFailed, failures))
        {
            Scene scene;
            GameObject* parentObject = scene.create("Controller test parent");
            parentObject->transform().position = {4.0f, 0.0f, 0.0f};
            parentObject->transform().rotation.y = 35.0f;
            GameObject* object = scene.create(parentObject, "Controller check");
            PlayerControllerComponent* player = object->addComponent<PlayerControllerComponent>();
            PlayerControllerComponentDesc playerDesc{};
            playerDesc.physics.position = {0.0f, 4.0f, 0.0f};
            playerDesc.physics.radius = 0.3f; playerDesc.physics.halfHeight = 0.6f;
            playerDesc.physics.jumpSpeed = 5.0f;
            player->create(playerDesc); player->ensurePhysicsInitialized(world);
            check(player->isPhysicsInitialized(), "controller component initialization", checksPassed, checksFailed, failures);
            check(near(player->getController().getPosition(), {4.0f, 4.0f, 0.0f}), "controller starts at parent-relative world position", checksPassed, checksFailed, failures);
            for (int i = 0; i < 180; ++i)
            {
                player->fixedUpdate(physics::kPhysicsDeltaTime); world.step();
            }
            check(player->getController().isOnGround(), "controller lands on floor", checksPassed, checksFailed, failures);
            const float groundedY = player->getController().getPosition().y;
            check(near(player->getCameraPosition(), player->getEyePosition()) && near(player->getCameraLookTarget(), player->getEyePosition()), "controller camera helpers agree", checksPassed, checksFailed, failures);
            check(std::abs(math::length(player->getCameraForward()) - 1.0f) < 0.001f, "controller camera forward is normalized", checksPassed, checksFailed, failures);
            const Transform simulatedPose = player->getWorldTransform();
            check(near(physics::toPhysicsTransform(object->transform()).position, simulatedPose.position), "controller synchronizes through rotated parent", checksPassed, checksFailed, failures);
            player->setJumpPressed(true);
            player->setJumpPressed(false);
            for (int i = 0; i < 10; ++i)
            {
                player->fixedUpdate(physics::kPhysicsDeltaTime); world.step();
            }
            check(player->getController().getPosition().y > groundedY + 0.1f, "jump press survives a false render-frame input", checksPassed, checksFailed, failures);
            playerDesc.physics.position = {3.0f, 4.0f, 0.0f};
            player->create(playerDesc);
            player->ensurePhysicsInitialized(world);
            check(near(player->getController().getPosition(), physics::toPhysicsTransform(object->transform()).position), "controller recreation applies new parent-relative position", checksPassed, checksFailed, failures);
            const vec3 relocated{2.0f, 4.0f, 1.0f};
            player->getController().setPosition(relocated);
            check(near(player->getController().getPosition(), relocated), "controller position getter / setter", checksPassed, checksFailed, failures);
            player->getController().setYaw(0.7f);
            check(std::abs(player->getController().getYaw() - 0.7f) < 0.001f, "controller yaw getter / setter", checksPassed, checksFailed, failures);
            const physics::BodyMeshData controllerMesh = player->getController().getMeshData(false);
            check(!controllerMesh.vertices.empty() && !controllerMesh.indices.empty(), "controller shape mesh extraction", checksPassed, checksFailed, failures);
            player->physicsShutdown();
            check(!player->isPhysicsInitialized(), "controller shutdown", checksPassed, checksFailed, failures);
            player->ensurePhysicsInitialized(world);
            check(player->isPhysicsInitialized(), "controller restart", checksPassed, checksFailed, failures);
            scene.clear();
            world.destroyBody(floorId);
        }

        world.drainEvents();
        physics::BodyDesc sensor = boxDesc({0.0f, 3.0f, 0.0f}, vec3{1.0f}, physics::MotionType::Static);
        sensor.isSensor = true;
        const physics::BodyId sensorId = world.createBody(sensor);
        physics::BodyDesc falling{};
        falling.motionType = physics::MotionType::Dynamic;
        falling.layer = physics::ObjectLayer::Moving;
        falling.shape.type = physics::ShapeType::Sphere;
        falling.shape.sphere.radius = 0.4f;
        falling.transform.position = {0.0f, 7.0f, 0.0f};
        const physics::BodyId fallingId = world.createBody(falling);
        if (sensorId != physics::InvalidBodyId && fallingId != physics::InvalidBodyId)
        {
            for (int i = 0; i < 150; ++i)
            {
                world.step();
            }
            bool added = false, removed = false;
            for (const auto& event : world.drainEvents())
            {
                const bool pair = (event.bodyA == sensorId && event.bodyB == fallingId) ||
                                  (event.bodyB == sensorId && event.bodyA == fallingId);
                if (pair && event.type == physics::PhysicsEventType::ContactAdded)
                {
                    added = true;
                }
                if (pair && event.type == physics::PhysicsEventType::ContactRemoved)
                {
                    removed = true;
                }
            }
            check(added && removed, "sensor contact start / end events", checksPassed, checksFailed, failures);
            check(world.getTransform(fallingId).position.y < 0.0f, "sensor allows body to pass through", checksPassed, checksFailed, failures);
        }
        else
        {
            check(false, "sensor test body creation", checksPassed, checksFailed, failures);
        }

        if (fallingId != physics::InvalidBodyId)
        {
            world.destroyBody(fallingId);
        }
        if (sensorId != physics::InvalidBodyId)
        {
            world.destroyBody(sensorId);
        }

        {
            const physics::BodyDesc platformDesc = boxDesc(
                {40.0f, -0.5f, 0.0f},
                {4.0f, 0.5f, 4.0f},
                physics::MotionType::Kinematic
            );

            const physics::BodyId platformId = world.createBody(platformDesc);

            if (check(platformId != physics::InvalidBodyId, "moving-platform creation", checksPassed, checksFailed, failures))
            {
                physics::PlayerController player;

                physics::PlayerControllerDesc playerDesc{};
                playerDesc.position = {40.0f, 3.0f, 0.0f};
                playerDesc.radius = 0.3f;
                playerDesc.halfHeight = 0.6f;
                playerDesc.jumpSpeed = 5.0f;

                if (check(player.init(world, playerDesc), "moving-platform controller initialization", checksPassed, checksFailed, failures))
                {
                    for (int i = 0; i < 180; ++i)
                    {
                        player.update({}, physics::kPhysicsDeltaTime);
                        world.step();
                    }

                    check(player.isOnGround(), "controller lands on kinematic platform", checksPassed, checksFailed, failures);

                    const float initialOffsetX = player.getPosition().x - world.getTransform(platformId).position.x;

                    world.setLinearVelocity(platformId, {2.0f, 0.0f, 0.0f});

                    for (int i = 0; i < 60; ++i)
                    {
                        player.update({}, physics::kPhysicsDeltaTime);
                        world.step();
                    }

                    const float finalOffsetX = player.getPosition().x - world.getTransform(platformId).position.x;

                    check(player.isOnGround() && std::abs(finalOffsetX - initialOffsetX) < 0.05f, "idle controller follows horizontal platform", checksPassed, checksFailed, failures);

                    const float initialOffsetY = player.getPosition().y - world.getTransform(platformId).position.y;

                    world.setLinearVelocity(platformId, {0.0f, 1.0f, 0.0f});

                    for (int i = 0; i < 60; ++i)
                    {
                        player.update({}, physics::kPhysicsDeltaTime);
                        world.step();
                    }

                    const float finalOffsetY = player.getPosition().y - world.getTransform(platformId).position.y;

                    check(player.isOnGround() && std::abs(finalOffsetY - initialOffsetY) < 0.05f, "idle controller follows vertical platform", checksPassed, checksFailed, failures);

                    const float groundedOffsetY = finalOffsetY;

                    physics::PlayerInput jump{};
                    jump.jumpPressed = true;

                    player.update(jump, physics::kPhysicsDeltaTime);
                    world.step();

                    for (int i = 0; i < 10; ++i)
                    {
                        player.update({}, physics::kPhysicsDeltaTime);
                        world.step();
                    }

                    const float jumpedOffsetY = player.getPosition().y - world.getTransform(platformId).position.y;

                    check(jumpedOffsetY > groundedOffsetY + 0.1f, "controller jumps from moving platform", checksPassed, checksFailed, failures);
                }

                player.shutdown();
                world.destroyBody(platformId);
            }
        }

        world.shutdown();
        world.shutdown();
        world.init();
        id = world.createBody(desc);
        check(id != physics::InvalidBodyId, "world shutdown and restart", checksPassed, checksFailed, failures);
        if (id != physics::InvalidBodyId)
        {
            world.destroyBody(id);
        }
        world.shutdown();

        if (checksFailed == 0)
        {
            logger::Log::infoFmt("[Physics Lab] {} passed, {} failed", checksPassed, checksFailed);
        }
        else
        {
            logger::Log::errorFmt("[Physics Lab] {} passed, {} failed", checksPassed, checksFailed);
        }

    }
}
