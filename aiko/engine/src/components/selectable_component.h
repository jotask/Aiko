#pragma once

#include <optional>

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

        void setStyle(const UISelectableStyle& style);
        void clearStyle();

        bool hasStyleOverride() const;

        const UISelectableStyle& resolveStyle(const UISelectableStyle& themeStyle) const;

    private:

        bool m_pointerInside = false;
        bool m_pressed = false;

        std::optional<UISelectableStyle> m_styleOverride;
    };

}
