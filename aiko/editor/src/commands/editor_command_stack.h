#pragma once

#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include <aiko_types.h>

#include "commands/editor_command.h"

namespace aiko::editor
{

    class EditorCommandStack
    {
    public:

        template<class T, class... Args> T& execute(Args&&... args)
        {
            static_assert(std::is_base_of_v<EditorCommand, T>, "EditorCommandStack::execute requires an EditorCommand type");
            auto command = std::make_unique<T>(std::forward<Args>(args)...);
            T& result = *command;
            execute(std::move(command));
            return result;
        }

        void execute(AikoUPtr<EditorCommand> command);

        bool canUndo() const;
        bool canRedo() const;

        void undo();
        void redo();

        void clear();

    private:
        vector<AikoUPtr<EditorCommand>> m_undoStack;
        vector<AikoUPtr<EditorCommand>> m_redoStack;
    };

}
