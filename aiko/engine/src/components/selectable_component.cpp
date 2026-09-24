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

        if (m_pressed && m_pointerInside)
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

    void SelectableComponent::setStyle(const UISelectableStyle& style)
    {
        m_styleOverride = style;
    }

    void SelectableComponent::clearStyle()
    {
        m_styleOverride.reset();
    }

    bool SelectableComponent::hasStyleOverride() const
    {
        return m_styleOverride.has_value();
    }

    const UISelectableStyle& SelectableComponent::resolveStyle(const UISelectableStyle& themeStyle) const
    {
        if (m_styleOverride.has_value())
        {
            return *m_styleOverride;
        }

        return themeStyle;
    }

}
