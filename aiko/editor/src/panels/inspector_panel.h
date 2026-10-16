#pragma once

#include "core/editor_panel.h"
#include "commands/transform/transform_state.h"
#include "core/uuid.h"

#include <optional>

namespace aiko
{
    class GameObject;
}

namespace aiko::editor
{
    class InspectorPanel final : public EditorPanel
    {
    public:
        InspectorPanel();
        void render(EditorContext& context) override;

    private:
        void finishTransformEdit(EditorContext& context, const TransformState& after);

        std::optional<uuid::Uuid> m_transformEditObjectId;
        GameObject* m_transformEditObject = nullptr;
        TransformState m_transformEditBefore;
    };
}
