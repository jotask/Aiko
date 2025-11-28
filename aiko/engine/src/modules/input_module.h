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

        InputModule(Aiko* aiko) : BaseModule(aiko) { };
        virtual ~InputModule() = default;

        bool isKeyPressed(Key) const;
        bool isKeyJustPressed(Key) const;
        vec2 getMousePosition() const;
        vec2 getMouseDelta() const;
        vec2 getMouseScrollBack() const;
        bool isMouseButtonPressed(MouseButton button) const;

        void setCentredToScreen(bool);
        bool getCentredToScreen() const;

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

        std::map<Key, InputType> m_keys_inputs;
        std::map<MouseButton, InputType> m_mouse_inputs;

        void onKeyPressed(Event& event);
        void onMouseKeyPressed(Event& event);
        void onMouseMoved(Event& event);
        void OnMouseScrollCallback(Event& event);

        InputType::PressedType convertToAction(int code);

        DisplayModule* m_displayModule;

        vec2 m_mousePosition;
        vec2 m_mouseScrollBack;
        vec2 m_mouseDelta;
        bool m_mouseCentred;
    };

}
