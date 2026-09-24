#include "button_component.h"

#include <utility>

namespace aiko
{

    ButtonComponent::ButtonComponent()
        : Component("Button")
    {
    }

    void ButtonComponent::setOnClick(ClickCallback callback)
    {
        m_onClick = std::move(callback);
    }

    UIEventPropagation ButtonComponent::onPointerEvent(const UIPointerEvent& event)
    {
        if (event.type != UIPointerEventType::Click)
        {
            return UIEventPropagation::Continue;
        }

        if (m_onClick)
        {
            m_onClick();
        }

        return UIEventPropagation::Stop;
    }

}
