#pragma once

#include "commands/editor_command.h"
#include "models/component.h"
#include "models/game_object.h"
#include "registry/components_functionality.h"
#include "systems/scene_system.h"

#include <yaml-cpp/yaml.h>

#include <type_traits>

namespace aiko::editor
{

    template<class T>
    class RemoveComponentCommand final : public EditorCommand
    {
    public:
        explicit RemoveComponentCommand(SceneSystem& sceneSystem, GameObject& object)
            : m_sceneSystem(&sceneSystem)
            , m_objectId(object.uuid())
        {
            static_assert(std::is_base_of_v<Component, T>, "RemoveComponentCommand requires a Component type");
            AIKO_ASSERT(object.hasComponent<T>(), "GameObject does not have this component");
        }

        void execute() override
        {
            GameObject* object = m_sceneSystem->findGameObject(m_objectId);
            AIKO_ASSERT(object != nullptr, "RemoveComponentCommand object no longer exists");
            if (object == nullptr)
            {
                return;
            }
            T* component = object->getComponent<T>();
            AIKO_ASSERT(component != nullptr, "GameObject does not have requested component");
            if (component == nullptr)
            {
                return;
            }
            if (m_hasSnapshot == false)
            {
                m_snapshot = YAML::Node{};
                const bool serialized = component::serializeComponent(*component, m_snapshot);
                AIKO_ASSERT(serialized, "Component cannot be serialized");
                if (serialized == false)
                {
                    return;
                }
                m_hasSnapshot = true;
            }
            object->removeComponent(component);
        }

        void undo() override
        {
            GameObject* object = m_sceneSystem->findGameObject(m_objectId);
            AIKO_ASSERT(object != nullptr, "RemoveComponentCommand object no longer exists");
            AIKO_ASSERT(m_hasSnapshot, "RemoveComponentCommand has no snapshot");
            if (object == nullptr || m_hasSnapshot == false)
            {
                return;
            }
            AIKO_ASSERT(object->hasComponent<T>() == false, "GameObject already has this component");
            const bool restored = component::deserializeComponent(m_snapshot, *object);
            AIKO_ASSERT(restored, "Component could not be restored");
        }

    private:
        SceneSystem* m_sceneSystem = nullptr;
        uuid::Uuid m_objectId;

        YAML::Node m_snapshot;
        bool m_hasSnapshot = false;
    };

}
