#pragma once

#include <aiko_types.h>

#include <vector>

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

        std::vector<GameObject*> getObjects() const;

        const std::vector<AikoPtr<GameObject>>& objects() const { return m_objects; }

    private:
        std::vector<AikoPtr<GameObject>> m_objects;
    };

}