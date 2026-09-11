#pragma once

#include <vector>

#include "aiko_types.h"
#include "application/application.h"

#include "windows/window.h"

#include "core/editor_context.h"

namespace aiko::editor
{

    class AikoEditor : public Layer
    {
    public:
        using Windows = std::vector<AikoUPtr<Window>>;

        AikoEditor();
        Aiko* getAiko() const;

        const Windows& getWindows() { return m_windows; }

        EditorContext& context() { return m_context; }
        const EditorContext& context() const { return m_context; }

    protected:
        virtual void init() override;
        virtual void render() override;

        EditorContext m_context;
        Windows m_windows;

    };

}

