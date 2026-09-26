#include "editor_workspace.h"

#include "core/editor_context.h"

namespace aiko::editor
{

    void EditorWorkspace::render(EditorContext& context)
    {
        for (auto& panel : m_panels)
        {
            if (panel != nullptr && panel->isOpen())
            {
                panel->render(context);
            }
        }
    }

}