#pragma once

#include <utility>
#include <memory>
#include <vector>
#include <type_traits>

#include "core/editor_panel.h"

namespace aiko::editor
{

    class EditorContext;

    class EditorWorkspace
    {
    public:
        template<class T, class... Args>
        T& addPanel(Args&&... args)
        {
            static_assert(std::is_base_of_v<EditorPanel, T>, "EditorWorkspace panels must derive from EditorPanel");
            auto panel = std::make_unique<T>(std::forward<Args>(args)...);
            T& result = *panel;
            m_panels.emplace_back(std::move(panel));
            return result;
        }

        void render(EditorContext& context);

        const vector<AikoUPtr<EditorPanel>>& panels() const
        {
            return m_panels;
        }

    private:
        vector<AikoUPtr<EditorPanel>> m_panels;
    };

}