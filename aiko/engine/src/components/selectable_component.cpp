#include "selectable_component.h"

#include <algorithm>

namespace aiko
{

    namespace
    {
        const UIImageAppearance& appearanceForState(const UISelectableStyle& style, UISelectableState state)
        {
            switch (state)
            {
                case UISelectableState::Normal:     return style.normal;
                case UISelectableState::Hovered:    return style.hovered;
                case UISelectableState::Pressed:    return style.pressed;
                case UISelectableState::Disabled:   return style.disabled;
            }
            return style.normal;
        }

        UIImageAppearance interpolateAppearance(const UIImageAppearance& from, const UIImageAppearance& to, float t)
        {
            UIImageAppearance result = to;
            result.color = Color::lerp(from.color, to.color, t);
            result.border.color = Color::lerp(from.border.color, to.border.color, t);
            result.border.thickness = math::lerp(from.border.thickness, to.border.thickness, t);
            result.cornerRadius = math::lerp(from.cornerRadius, to.cornerRadius, t);
            return result;
        }
    }

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

    UIImageAppearance SelectableComponent::resolveAppearance(const UISelectableStyle& style, float deltaTime)
    {
        const UISelectableState state = getState();

        const UIImageAppearance& target = appearanceForState(style, state);

        if (m_transitionInitialized == false)
        {
            m_visualState = state;
            m_transitionSource = target;
            m_transitionElapsed = style.transitionDuration;
            m_transitionInitialized = true;

            return target;
        }

        if (style.transitionDuration <= 0.0f)
        {
            m_visualState = state;
            m_transitionSource = target;
            m_transitionElapsed = 0.0f;

            return target;
        }

        if (state != m_visualState)
        {
            const UIImageAppearance& previousTarget = appearanceForState(style, m_visualState);

            const float previousT = std::clamp(m_transitionElapsed / style.transitionDuration, 0.0f, 1.0f);

            m_transitionSource = interpolateAppearance(m_transitionSource, previousTarget, previousT);

            m_visualState = state;
            m_transitionElapsed = 0.0f;
        }

        m_transitionElapsed = std::min(m_transitionElapsed + std::max(deltaTime, 0.0f), style.transitionDuration);

        const float t = std::clamp(m_transitionElapsed / style.transitionDuration, 0.0f, 1.0f);

        return interpolateAppearance(m_transitionSource, target, t);
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
