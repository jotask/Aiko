#pragma once

#include <aiko_types.h>

namespace aiko
{
    namespace editor
    {
        class AikoEditor;
        class MenuItem
        {
        public:
            MenuItem(AikoEditor*, const char*, bool = false);
            ~MenuItem() = default;
            virtual void render() = 0 ;
            const char* getName() { return name.c_str(); };
            bool is_open;
        protected:
            AikoEditor* m_editor;
            aiko::string name;
        };

    }
}
