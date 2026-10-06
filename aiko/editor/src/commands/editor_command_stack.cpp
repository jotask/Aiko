#include "editor_command_stack.h"

#include <utility>

namespace aiko::editor
{

    void EditorCommandStack::execute(AikoUPtr<EditorCommand> command)
    {
        if (command == nullptr)
        {
            return;
        }
        command->execute();
        m_undoStack.emplace_back(std::move(command));
        m_redoStack.clear();

        notifyMutation();
    }

    void EditorCommandStack::pushExecuted(AikoUPtr<EditorCommand> command)
    {
        if (command == nullptr)
        {
            return;
        }
        m_undoStack.emplace_back(std::move(command));
        m_redoStack.clear();

        notifyMutation();
    }

    bool EditorCommandStack::canUndo() const
    {
        return m_undoStack.empty() == false;
    }

    bool EditorCommandStack::canRedo() const
    {
        return m_redoStack.empty() == false;
    }

    void EditorCommandStack::undo()
    {
        if (m_undoStack.empty())
        {
            return;
        }
        AikoUPtr<EditorCommand> command = std::move(m_undoStack.back());
        m_undoStack.pop_back();
        command->undo();
        m_redoStack.emplace_back(std::move(command));

        notifyMutation();
    }

    void EditorCommandStack::redo()
    {
        if (m_redoStack.empty())
        {
            return;
        }
        AikoUPtr<EditorCommand> command = std::move(m_redoStack.back());
        m_redoStack.pop_back();
        command->execute();
        m_undoStack.emplace_back(std::move(command));

        notifyMutation();
    }

    void EditorCommandStack::clear()
    {
        m_undoStack.clear();
        m_redoStack.clear();
    }

    void EditorCommandStack::notifyMutation()
    {
        if (m_mutationCallback)
        {
            m_mutationCallback();
        }
    }

}
