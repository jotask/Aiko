#pragma once

#include <aiko_types.h>
#include <math/math.h>

#include "models/component.h"

namespace aiko
{

    enum class CanvasScaleMode
    {
        ConstantPixelSize,
        ScaleWithScreenSize
    };

    class CanvasComponent : public Component
    {
    public:

        CanvasComponent();
        virtual ~CanvasComponent() = default;

        void setScaleMode(CanvasScaleMode mode)
        {
            m_scaleMode = mode;
        }

        CanvasScaleMode getScaleMode() const
        {
            return m_scaleMode;
        }

        void setReferenceResolution(const vec2& resolution)
        {
            AIKO_ASSERT(resolution.x > 0.0f, "Canvas reference width must be greater than zero");
            AIKO_ASSERT(resolution.y > 0.0f, "Canvas reference height must be greater than zero");

            m_referenceResolution = resolution;
        }

        const vec2& getReferenceResolution() const
        {
            return m_referenceResolution;
        }

        void setMatchWidthOrHeight(float match)
        {
            AIKO_ASSERT(match >= 0.0f && match <= 1.0f, "Canvas width/height match must be between zero and one");

            m_matchWidthOrHeight = match;
        }

        float getMatchWidthOrHeight() const
        {
            return m_matchWidthOrHeight;
        }

        void setScaleFactor(float scale)
        {
            AIKO_ASSERT(scale > 0.0f, "Canvas scale factor must be greater than zero");

            m_scaleFactor = scale;
        }

        float getScaleFactor() const
        {
            return m_scaleFactor;
        }

        void setSortingOrder(int32_t order)
        {
            m_sortingOrder = order;
        }

        int32_t getSortingOrder() const
        {
            return m_sortingOrder;
        }

    private:

        CanvasScaleMode m_scaleMode = CanvasScaleMode::ScaleWithScreenSize;

        vec2 m_referenceResolution = {1920.0f, 1080.0f};

        float m_matchWidthOrHeight = 0.5f;
        float m_scaleFactor = 1.0f;

        int32_t m_sortingOrder = 0;
    };

}
