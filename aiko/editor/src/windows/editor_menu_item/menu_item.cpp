#include "menu_item.h"

namespace aiko
{
    namespace editor
    {

        MenuItem::MenuItem(AikoEditor* editor, const char* name, bool open)
            : m_editor(editor)
            , name(name)
            , is_open(open)
        {

        }

    }
}
