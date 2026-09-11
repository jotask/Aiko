#pragma once

#include "window.h"

namespace aiko
{
    class GameObject;
    class Scene;
    namespace editor
    {
        class AikoEditor;
        class HirearchyWindow : public Window
        {
        public:
            HirearchyWindow(AikoEditor*);
            virtual ~HirearchyWindow() = default;

            virtual void init() override;
            virtual void render() override;

        private:
            void renderGameObject(Scene& scene, GameObject* obj);
            GameObject* findGameObjectByTransform(Scene& scene, Transform* transform);

            void attachChild(GameObject* parent, GameObject* child);
            bool canAttachChild(GameObject* parent, GameObject* child) const;

            void detachFromParent(GameObject* child);

            GameObject* m_renameTarget = nullptr;
            GameObject* m_renameJustStarted = nullptr;
            string m_renameBuffer;

        };

    }
}
