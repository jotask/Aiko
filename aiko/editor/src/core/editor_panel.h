#pragma once

#include <aiko_types.h>
#include <utility>

namespace aiko::editor
{

    class EditorContext;

    class EditorPanel
    {
    public:
        explicit EditorPanel(string name, bool open = true);
        virtual ~EditorPanel() = default;

        virtual void render(EditorContext& context) = 0;

        const string& name() const
        {
            return m_name;
        }

        bool isOpen() const
        {
            return m_open;
        }

        bool& openState()
        {
            return m_open;
        }

        void setOpen(bool open)
        {
            m_open = open;
        }

    private:
        string m_name;
        bool m_open = true;
    };

}
