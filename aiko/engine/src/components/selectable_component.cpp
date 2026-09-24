#include "selectable_component.h"

namespace aiko
{

    SelectableComponent::SelectableComponent()
        : Component("Selectable")
    {
    }

    UISelectableState SelectableComponent::getState() const
    {
        if (isActiveAndEnabled() == false)
        {
            return UISelectableState::Disabled;
        }

        if (m_pressed)
        {
            return UISelectableState::Pressed;
        }

        if (m_pointerInside)
        {
            return UISelectableState::Hovered;
        }

        return UISelectableState::Normal;
    }

    const UIImageAppearance& SelectableComponent::resolveAppearance(const UISelectableStyle& style) const
    {
        switch (getState())
        {
            case UISelectableState::Normal:     return style.normal;
            case UISelectableState::Hovered:    return style.hovered;
            case UISelectableState::Pressed:    return style.pressed;
            case UISelectableState::Disabled:   return style.disabled;
        }
        return style.normal;
    }

    UIEventPropagation  SelectableComponent::onPointerEvent(const UIPointerEvent& event)
    {
        switch (event.type)
        {
            case UIPointerEventType::Enter:
                m_pointerInside = true;
                break;

            case UIPointerEventType::Exit:
                m_pointerInside = false;
                m_pressed = false;
                break;

            case UIPointerEventType::Down:
                m_pressed = true;
                break;

            case UIPointerEventType::Up:
                m_pressed = false;
                break;

            case UIPointerEventType::Click:
                break;
        }

        return UIEventPropagation::Continue;
    }

}
