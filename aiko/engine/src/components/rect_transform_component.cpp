#include "rect_transform_component.h"

namespace aiko
{

    RectTransformComponent::RectTransformComponent()
        : Component("Rect Transform")
    {
    }

    void RectTransformComponent::setAnchors(const vec2& min, const vec2& max)
    {
        AIKO_ASSERT(min.x >= 0.0f && min.x <= 1.0f, "RectTransform anchor min x must be between zero and one");
        AIKO_ASSERT(min.y >= 0.0f && min.y <= 1.0f, "RectTransform anchor min y must be between zero and one");

        AIKO_ASSERT(max.x >= 0.0f && max.x <= 1.0f, "RectTransform anchor max x must be between zero and one");
        AIKO_ASSERT(max.y >= 0.0f && max.y <= 1.0f, "RectTransform anchor max y must be between zero and one");

        AIKO_ASSERT(min.x <= max.x, "RectTransform anchor min x must not exceed anchor max x");
        AIKO_ASSERT(min.y <= max.y, "RectTransform anchor min y must not exceed anchor max y");

        m_anchorMin = min;
        m_anchorMax = max;
    }

    void RectTransformComponent::setAnchorMin(const vec2& anchor)
    {
        AIKO_ASSERT(anchor.x >= 0.0f && anchor.x <= 1.0f, "RectTransform anchor min x must be between zero and one");
        AIKO_ASSERT(anchor.y >= 0.0f && anchor.y <= 1.0f, "RectTransform anchor min y must be between zero and one");

        AIKO_ASSERT(anchor.x <= m_anchorMax.x, "RectTransform anchor min x must not exceed anchor max x");
        AIKO_ASSERT(anchor.y <= m_anchorMax.y, "RectTransform anchor min y must not exceed anchor max y");

        m_anchorMin = anchor;
    }

    void RectTransformComponent::setAnchorMax(const vec2& anchor)
    {
        AIKO_ASSERT(anchor.x >= 0.0f && anchor.x <= 1.0f, "RectTransform anchor max x must be between zero and one");
        AIKO_ASSERT(anchor.y >= 0.0f && anchor.y <= 1.0f, "RectTransform anchor max y must be between zero and one");

        AIKO_ASSERT(anchor.x >= m_anchorMin.x, "RectTransform anchor max x must not be less than anchor min x");
        AIKO_ASSERT(anchor.y >= m_anchorMin.y, "RectTransform anchor max y must not be less than anchor min y");

        m_anchorMax = anchor;
    }

    void RectTransformComponent::setPivot(const vec2& pivot)
    {
        AIKO_ASSERT(pivot.x >= 0.0f && pivot.x <= 1.0f, "RectTransform pivot x must be between zero and one");
        AIKO_ASSERT(pivot.y >= 0.0f && pivot.y <= 1.0f, "RectTransform pivot y must be between zero and one");

        m_pivot = pivot;
    }

}
