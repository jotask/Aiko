#pragma once

#include <string>

namespace aiko
{
    namespace editor
    {
        class AikoEditor;
        class MenuItem
        {
        public:
            MenuItem(AikoEditor*, const char*);
            ~MenuItem() = default;
            virtual void render() = 0 ;
            const char* getName() { return name.c_str(); };
        protected:
            AikoEditor* m_editor;
            bool is_open = true;
            std::string name;
        };

    }
}
