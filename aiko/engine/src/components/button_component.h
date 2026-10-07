#pragma once

#include <functional>

#include "models/component.h"
#include "ui/ui_pointer_event.h"

namespace aiko
{

    class ButtonComponent : public Component, public UIPointerEventHandler
    {
    public:

        using ClickCallback = std::function<void()>;

        ButtonComponent();
        ~ButtonComponent() override = default;

        void setOnClick(ClickCallback callback);

        UIEventPropagation onPointerEvent(const UIPointerEvent& event) override;

    private:

        ClickCallback m_onClick;
    };

}
