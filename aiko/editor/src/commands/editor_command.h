#pragma once

namespace aiko::editor
{

    class EditorCommand
    {
    public:
        virtual ~EditorCommand() = default;

        virtual void execute() = 0;
        virtual void undo() = 0;
    };

}
