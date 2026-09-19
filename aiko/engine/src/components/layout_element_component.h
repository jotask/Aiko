#pragma once

#include <aiko_types.h>
#include <math/math.h>

#include "models/component.h"

namespace aiko
{

    class LayoutElementComponent : public Component
    {
    public:

        LayoutElementComponent();
        virtual ~LayoutElementComponent() = default;

        void setMinSize(const vec2& size)
        {
            AIKO_ASSERT(size.x >= 0.0f && size.y >= 0.0f,
                "Layout minimum size must not be negative");

            m_minSize = size;
        }

        const vec2& getMinSize() const
        {
            return m_minSize;
        }

        void setPreferredSize(const vec2& size)
        {
            AIKO_ASSERT(size.x >= 0.0f && size.y >= 0.0f,
                "Layout preferred size must not be negative");

            m_preferredSize = size;
        }

        const vec2& getPreferredSize() const
        {
            return m_preferredSize;
        }

        void setFlexibleWeight(const vec2& weight)
        {
            AIKO_ASSERT(weight.x >= 0.0f && weight.y >= 0.0f, "Layout flexible weight must not be negative");
            m_flexibleWeight = weight;
        }

        const vec2& getFlexibleWeight() const
        {
            return m_flexibleWeight;
        }

    private:

        vec2 m_minSize = {0.0f, 0.0f};
        vec2 m_preferredSize = {0.0f, 0.0f};
        vec2 m_flexibleWeight = {0.0f, 0.0f};
    };

}
