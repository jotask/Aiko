#pragma once

#include "commands/editor_command.h"
#include "models/component.h"
#include "models/game_object.h"
#include "systems/scene_system.h"

#include <type_traits>

namespace aiko::editor
{

    template<class T>
    class AddComponentCommand final : public EditorCommand
    {
    public:
        explicit AddComponentCommand(SceneSystem& sceneSystem, GameObject& object)
            : m_sceneSystem(&sceneSystem)
            , m_objectId(object.uuid())
        {
            static_assert(std::is_base_of_v<Component, T>, "AddComponentCommand requires a Component type");
            AIKO_ASSERT(object.hasComponent<T>() == false, "GameObject already has this component");
        }

        void execute() override
        {
            GameObject* object = m_sceneSystem->findGameObject(m_objectId);
            AIKO_ASSERT(object != nullptr, "AddComponentCommand object no longer exists");
            if (object == nullptr)
            {
                return;
            }
            AIKO_ASSERT(object->hasComponent<T>() == false, "GameObject already has this component");
            object->addComponent<T>();
        }

        void undo() override
        {
            GameObject* object = m_sceneSystem->findGameObject(m_objectId);
            AIKO_ASSERT(object != nullptr, "AddComponentCommand object no longer exists");
            if (object == nullptr)
            {
                return;
            }
            T* component = object->getComponent<T>();
            AIKO_ASSERT(component != nullptr, "AddComponentCommand component no longer exists");
            if (component == nullptr)
            {
                return;
            }
            object->removeComponent(component);
        }

    private:
        SceneSystem* m_sceneSystem = nullptr;
        uuid::Uuid m_objectId;
    };

}
