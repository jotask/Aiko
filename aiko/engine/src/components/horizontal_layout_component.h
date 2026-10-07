#pragma once

#include <aiko_types.h>

#include "models/component.h"
#include "ui/ui_layout.h"

namespace aiko
{

    class HorizontalLayoutComponent : public Component
    {
    public:

        HorizontalLayoutComponent();
        virtual ~HorizontalLayoutComponent() = default;

        void setSpacing(float spacing)
        {
            AIKO_ASSERT(spacing >= 0.0f, "Horizontal layout spacing must not be negative");
            m_spacing = spacing;
        }

        float getSpacing() const
        {
            return m_spacing;
        }

        void setPadding(const UIPadding& padding)
        {
            AIKO_ASSERT(
                padding.left >= 0.0f &&
                padding.right >= 0.0f &&
                padding.top >= 0.0f &&
                padding.bottom >= 0.0f,
                "Horizontal layout padding must not be negative");

            m_padding = padding;
        }

        const UIPadding& getPadding() const
        {
            return m_padding;
        }

        void setChildAlignment(UICrossAxisAlignment alignment)
        {
            m_childAlignment = alignment;
        }

        UICrossAxisAlignment getChildAlignment() const
        {
            return m_childAlignment;
        }

    private:

        float m_spacing = 0.0f;
        UIPadding m_padding;
        UICrossAxisAlignment m_childAlignment = UICrossAxisAlignment::Start;
    };

}
