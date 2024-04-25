#pragma once

#include <map>

#include "base_module.h"
#include "types/inputs.h"
#include "shared/math.h"
#include "events/event.hpp"

namespace aiko
{
    class DisplayModule;
    class InputModule : public BaseModule
    {
    public:
    
        InputModule() = default;
        virtual ~InputModule() = default;

        bool isKeyPressed(Key) const;
        bool isKeyJustPressed(Key) const;
        vec2 getMousePosition() const;
        bool isMouseButtonPressed(MouseButton button) const;

    protected:

        virtual void connect(ModuleConnector*);

        virtual void init() override;
        virtual void endFrame() override;

    private:

        struct InputType
        {
            enum class PressedType
            {
                RELEASE, // The key or mouse button was released.
                PRESS, // The key or mouse button was pressed.
                REPEAT, // The key was held down until it repeated.
            };
            PressedType Type;
            bool justPressed;
        };

        std::map<Key, InputType> m_inputs;

        void onKeyPressed(Event& event);

        DisplayModule* m_displayModule;
    };

}
