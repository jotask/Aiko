#pragma once

namespace aiko
{
    namespace editor
    {
        class AikoEditor;
        class Window
        {
        public:
            Window(AikoEditor*);
            ~Window() = default;
            virtual void render() = 0 ;
        protected:
            AikoEditor* m_editor;
        };

    }
}
