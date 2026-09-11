#pragma once

namespace aiko
{
    class GameObject;
    namespace editor
    {
        class EditorContext
        {
        public:
            GameObject* getSelectedGameObject() const { return m_selectedGameObject; }
            void setSelectedGameObject(GameObject* value) { m_selectedGameObject = value; }

        private:
            GameObject* m_selectedGameObject = nullptr;
        };
    }
}