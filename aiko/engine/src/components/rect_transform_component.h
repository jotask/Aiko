#pragma once

#include <aiko_types.h>
#include <math/math.h>

#include "models/component.h"

namespace aiko
{

    class RectTransformComponent : public Component
    {
    public:

        RectTransformComponent();
        virtual ~RectTransformComponent() = default;

        void setAnchors(const vec2& min, const vec2& max);

        void setAnchorMin(const vec2& anchor);
        void setAnchorMax(const vec2& anchor);

        const vec2& getAnchorMin() const { return m_anchorMin; }
        const vec2& getAnchorMax() const { return m_anchorMax; }

        void setPivot(const vec2& pivot);
        const vec2& getPivot() const { return m_pivot; }

        void setAnchoredPosition(const vec2& position)
        {
            m_anchoredPosition = position;
        }

        const vec2& getAnchoredPosition() const
        {
            return m_anchoredPosition;
        }

        void setSizeDelta(const vec2& size)
        {
            m_sizeDelta = size;
        }

        const vec2& getSizeDelta() const
        {
            return m_sizeDelta;
        }

    private:

        vec2 m_anchorMin = {0.5f, 0.5f};
        vec2 m_anchorMax = {0.5f, 0.5f};

        vec2 m_pivot = {0.5f, 0.5f};

        vec2 m_anchoredPosition = {0.0f, 0.0f};
        vec2 m_sizeDelta = {100.0f, 100.0f};
    };

}
