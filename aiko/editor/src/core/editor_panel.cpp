#include "editor_panel.h"

namespace aiko::editor
{

    EditorPanel::EditorPanel(string name, bool open)
        : m_name(std::move(name))
        , m_open(open)
    {
    }

}
