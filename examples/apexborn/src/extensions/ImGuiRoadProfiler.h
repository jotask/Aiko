#pragma once

#include "math/math_vector.h"
#include "world/road_profile.h"

#include <imgui.h>
#include <limits>
#include <optional>
#include <vector>

namespace ab
{
    struct RoadProfile;

    class ImGuiRoadProfiler final
    {
    public:

        ImGuiRoadProfiler() = default;
        ~ImGuiRoadProfiler() = default;

        void open(const RoadProfile& profile);
        void render(RoadProfile& destination);

        bool isOpen() const { return m_open; }

    private:

        struct PolylineProjection
        {
            aiko::vec2 position = {};
            float distance = std::numeric_limits<float>::infinity();
            size_t segmentIndex = 0;
        };

        RoadProfile m_editProfile;
        bool m_open = false;
        bool m_saveFailed = false;
        ImVec2 m_scrolling = {0.0f, 0.0f};

        std::optional<size_t> m_draggedControlPoint;
        std::optional<size_t> m_contextPointIndex;

        PolylineProjection GetClosestPointOnPolyline(const RoadProfile& roadProfiler, const aiko::vec2& mousePosInCanvas);
        std::optional<size_t> FindClosestControlPoint(const RoadProfile& roadProfiler, const aiko::vec2& mousePosInCanvas);

        void EvaluateSegmentProjection(const aiko::vec2& mousePosition, const aiko::vec2& pointA, const aiko::vec2& pointB, size_t segmentIdx, PolylineProjection& result);

    };
}