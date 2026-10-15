#pragma once

#include <core/transform.h>
#include <types/color.h>

#include "assets/asset_id.h"
#include "components/player_controller_component.h"
#include "events/physics_events.h"
#include "layers/layer.h"

#include <aiko_body_desc.h>
#include <aiko_query_types.h>

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace aiko
{
    class CameraComponent;
    class GameObject;
    class PhysicsSystem;
    class RenderSystem;
    class RigidBodyComponent;
    class SceneSystem;
    class SystemConnector;
}

namespace aiko::lab
{
    class PhysicsLab final : public Layer
    {
    public:
        void connect(SystemConnector& connector) override;
        void init() override;
        void update() override;
        void render() override;
        void dispose() override;

    private:
        enum class Action
        {
            None, ResetScene, RunChecks, Impulse, Force, Torque,
            AngularImpulse, LinearVelocity, AngularVelocity, Stop,
            Teleport, Recreate, ToggleEnabled, Remove, Static,
            Kinematic, Dynamic, ToggleSensor, Gravity,
            RecreateController, ToggleController, DeleteOnContact, Stall
        };

        struct BodyEntry
        {
            std::string name;
            uuid::Uuid objectId;
            physics::BodyDesc initial;
        };

        void buildScene();
        GameObject* addBody(const char* name, physics::BodyDesc desc, Color color, GameObject* parent = nullptr);
        GameObject* find(const uuid::Uuid& id) const;
        RigidBodyComponent* selectedBody() const;
        PlayerControllerComponent* controller() const;
        void recreate(BodyEntry& entry);
        void applyAction();
        void updateController();
        void updateQueries();
        void drawPanel();
        void drawQueries();

        void onContact(const PhysicsContactStartedEvent& event);
        void observeContact(const PhysicsContactStartedEvent& event);
        void onContactEnded(const PhysicsContactEndedEvent& event);

        PhysicsSystem* m_physicsSystem = nullptr;
        SceneSystem* m_sceneSystem = nullptr;
        RenderSystem* m_renderSystem = nullptr;
        CameraComponent* m_camera = nullptr;
        AssetId m_modelShader = InvalidAssetId;
        std::vector<BodyEntry> m_bodies;
        std::optional<uuid::Uuid> m_controllerId;
        std::optional<uuid::Uuid> m_platformId;
        std::optional<uuid::Uuid> m_deleteOnContact;
        PlayerControllerComponentDesc m_controllerDesc{};

        Action m_action = Action::None;
        int m_selected = 0;
        vec3 m_vector{0.0f, 8.0f, 0.0f};
        float m_gravityFactor = 1.0f;
        float m_platformTime = 0.0f;
        bool m_animatePlatform = false;
        bool m_debug = false;
        bool m_followController = false;
        bool m_thirdPerson = true;
        bool m_showQueries = true;

        physics::RayCastDesc m_ray{};
        physics::RayCastHit m_rayHit{};
        bool m_rayHasHit = false;
        physics::SphereOverlapDesc m_sphereQuery{};
        physics::BoxOverlapDesc m_boxQuery{};
        std::vector<physics::BodyId> m_sphereHits;
        std::vector<physics::BodyId> m_boxHits;

        std::uint64_t m_contactsStarted = 0;
        std::uint64_t m_contactsEnded = 0;
        std::uint64_t m_missingInObserver = 0;
        int m_checksPassed = 0;
        int m_checksFailed = 0;
        std::vector<std::string> m_failures;
    };
}

