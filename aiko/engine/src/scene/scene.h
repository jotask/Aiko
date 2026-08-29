#pragma once

#include <aiko_types.h>

#include <entt/entt.hpp>

namespace aiko
{

    class GameObject;

    class Scene
    {
    public:
        Scene() = default;
        ~Scene() = default;

        void add(const AikoPtr<GameObject>& obj);
        bool remove(const GameObject* obj);

        void clear();

        vector<GameObject*> getObjects() const;

        const vector<AikoPtr<GameObject>>& objects() const { return m_objects; }

        void setActiveCamera(GameObject* obj);
        auto getActiveCamera() const -> GameObject* { return m_activeCamera; }

        entt::registry& registry() { return m_registry; }
        const entt::registry& registry() const { return m_registry; }

    private:
        entt::registry m_registry;

        vector<AikoPtr<GameObject>> m_objects;
        GameObject* m_activeCamera = nullptr;
    };

}