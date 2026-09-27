#pragma once

#include <aiko_types.h>

#include "core/editor_panel.h"

namespace aiko
{
    class GameObject;
    class Scene;
}

namespace aiko::editor
{

    class HierarchyPanel final : public EditorPanel
    {
    public:
        HierarchyPanel();

        void render(EditorContext& context) override;

    private:
        void renderGameObject(Scene& scene, GameObject* object, EditorContext& context);

        void attachChild(EditorContext& context, GameObject* parent, GameObject* child);

        bool canAttachChild( GameObject* parent,GameObject* child) const;

        void detachFromParent(EditorContext& context, GameObject* child);

        GameObject* m_renameTarget = nullptr;
        GameObject* m_renameJustStarted = nullptr;
        string m_renameBuffer;
    };

}