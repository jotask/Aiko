#pragma once

#include <aiko_includes.h>

#include "core/editor_context.h"

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
            AikoEditor* getEditor() const { return m_editor; };
            Aiko* getAiko() const;
            EditorContext& context();
            const EditorContext& context() const;
        private:
            AikoEditor* m_editor;
            aiko::string name;
        };

    }
}
