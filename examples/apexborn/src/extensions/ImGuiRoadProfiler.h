#pragma once

#include "math/math_vector.h"

#include <imgui.h>
#include <limits>
#include <vector>
#include <optional>

namespace ab
{

    class ImGuiRoadProfiler final
    {
    public:

        static void Render();

        static const std::vector<aiko::vec2>& GetPoints();
        static bool IsClosed();

    private:
        ImGuiRoadProfiler() = default;

        struct PolylineProjection
        {
            aiko::vec2 position = {};
            float distance = std::numeric_limits<float>::infinity();
            size_t segmentIndex = 0;
        };

        static std::vector<aiko::vec2> points;
        static ImVec2 scrolling;
        static std::optional<size_t> draggedControlPoint;
        static std::optional<size_t> contextPointIndex;
        static bool isClosed;

        static PolylineProjection GetClosestPointOnPolyline(const aiko::vec2& mousePosInCanvas);
        static std::optional<size_t> FindClosestControlPoint(const aiko::vec2& mousePosInCanvas);

        static void EvaluateSegmentProjection(const aiko::vec2& mousePosition, const aiko::vec2& pointA, const aiko::vec2& pointB, size_t segmentIdx, PolylineProjection& result);

    };
}