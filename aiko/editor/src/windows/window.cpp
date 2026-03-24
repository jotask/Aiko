#include "window.h"

#include "aiko_editor.h"
#include "core/editor_context.h"

namespace aiko
{
    namespace editor
    {

        Window::Window(AikoEditor* editor, const char* name)
            : m_editor(editor)
            , name(name)
        {

        }

        Aiko* Window::getAiko() const
        {
            return m_editor->getAiko();
        }

        EditorContext& Window::context()
        {
            return m_editor->context();
        }

        const EditorContext& Window::context() const
        {
            return m_editor->context();
        }

    }
}
