#pragma once

#include "models/component.h"
#include "ui/ui_pointer_event.h"
#include "ui/ui_style.h"

namespace aiko
{

    enum class UISelectableState
    {
        Normal,
        Hovered,
        Pressed,
        Disabled
    };

    class SelectableComponent : public Component, public UIPointerEventHandler
    {
    public:

        SelectableComponent();
        ~SelectableComponent() override = default;

        UISelectableState getState() const;

        const UIImageAppearance& resolveAppearance(const UISelectableStyle& style) const;

        UIEventPropagation onPointerEvent(const UIPointerEvent& event) override;

    private:

        bool m_pointerInside = false;
        bool m_pressed = false;
    };

}
