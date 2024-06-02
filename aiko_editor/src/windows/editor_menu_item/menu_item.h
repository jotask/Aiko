#pragma once

#include <std::string>

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
            std::string name;
        };

    }
}
