#pragma once

#include <string>

namespace aiko
{
    namespace editor
    {
        class AikoEditor;
        class Window
        {
        public:
            Window(AikoEditor*, const char*);
            ~Window() = default;
            virtual void render() = 0;
            bool isOpen() const { return is_open; };
            const char* getName() const { return name.c_str(); }
            bool is_open = true;
        protected:
            AikoEditor* m_editor;
            std::string name;
        };

    }
}
