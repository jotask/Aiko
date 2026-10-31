#include "ImGuiRoadProfiler.h"

#include "constant_types.h"
#include "world/road_profile.h"

#include "aiko_types.h"
#include "math/math.h"

#include <cmath>

namespace ab
{

    namespace
    {
        constexpr float PolylineSegmentThickness = 2.5f;
        constexpr float ControlPointSize = 4.0f;
        constexpr float SnapDistance = 13.0f;
        constexpr float SelectedNodeRadius = 13.0f;
        constexpr float PixelsPerUnit = 160.0f;

        constexpr ImU32 CanvasBackground = IM_COL32(30, 35, 45, 255);
        constexpr ImU32 CanvasBorder = IM_COL32(85, 100, 120, 255);
        constexpr ImU32 GridColor = IM_COL32(110, 125, 145, 30);
        constexpr ImU32 PolylineColor = IM_COL32(100, 200, 245, 255);
        constexpr ImU32 ControlPointColor = IM_COL32(100, 200, 245, 255);
        constexpr ImU32 HoveredPointColor = IM_COL32(255, 205, 90, 255);
        constexpr ImU32 DraggedPointColor = IM_COL32(255, 125, 100, 255);
        constexpr ImU32 SnapPreviewColor = IM_COL32(90, 230, 155, 255);
        constexpr ImU32 ControlPointOutline = IM_COL32(15, 20, 30, 255);

    }

    void ImGuiRoadProfiler::open(const RoadProfile& profile)
    {
        m_editProfile = profile;
        m_draggedControlPoint.reset();
        m_contextPointIndex.reset();
        m_open = true;
        m_saveFailed = false;
    }

    void ImGuiRoadProfiler::render(RoadProfile& destination)
    {

        if (!m_open)
        {
            return;
        }

        if (!ImGui::Begin("Road Profile Editor", &m_open))
        {
            ImGui::End();
            return;
        }

        std::vector<aiko::vec2>& points = m_editProfile.points;
        bool& isClosed = m_editProfile.closed;

        if (ImGui::BeginTable("RoadProfilerToolbar", 2, ImGuiTableFlags_SizingStretchSame))
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            if (ImGui::BeginChild("Settings", ImVec2(0, 140), ImGuiChildFlags_Borders))
            {
                ImGui::SeparatorText("Settings");

                ImGui::TextDisabled("%zu points | %s", points.size(), isClosed ? "Closed" : "Open");

                ImGui::BeginDisabled(points.empty());
                if (ImGui::Button("Clear All"))
                {
                    points.clear();
                    m_draggedControlPoint.reset();
                    m_contextPointIndex.reset();
                    isClosed = false;
                }
                ImGui::EndDisabled();

                if (ImGui::Button("Save Profile"))
                {
                    RoadProfile normalized = m_editProfile;

                    if (normalized.normalizeWidth())
                    {
                        destination = normalized;
                        m_editProfile = std::move(normalized);
                        m_saveFailed = false;
                    }
                    else
                    {
                        m_saveFailed = true;
                    }
                }

                if (m_saveFailed)
                {
                    ImGui::TextColored(ImVec4(1.0f, 0.35f, 0.35f, 1.0f), "Invalid road profile");
                }

            }
            ImGui::EndChild();

            ImGui::TableSetColumnIndex(1);
            if (ImGui::BeginChild("Controls", ImVec2(0, 140), ImGuiChildFlags_Borders))
            {
                ImGui::SeparatorText("Controls");
                ImGui::BulletText("Left Click: Add or insert point");
                ImGui::BulletText("Ctrl + Left Drag: Move point");
                ImGui::BulletText("Ctrl + Click First Point: Close profile");
                ImGui::BulletText("Right Click: Delete point (context menu)");
                ImGui::BulletText("Right Drag: Pan canvas");
            }
            ImGui::EndChild();
            ImGui::EndTable();
        }

        ImGui::Spacing();

        const ImVec2 canvasP0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
        ImVec2 canvasSZ = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
        if (canvasSZ.x < 50.0f)
        {
            canvasSZ.x = 50.0f;
        }
        if (canvasSZ.y < 50.0f)
        {
            canvasSZ.y = 50.0f;
        }
        const ImVec2 canvasP1 = ImVec2(canvasP0.x + canvasSZ.x, canvasP0.y + canvasSZ.y);

        // Draw border and background color
        const ImGuiIO& io = ImGui::GetIO();
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(canvasP0, canvasP1, CanvasBackground);
        drawList->AddRect(canvasP0, canvasP1, CanvasBorder);

        // This will catch our interactions
        ImGui::InvisibleButton("canvas", canvasSZ, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
        const bool isHovered = ImGui::IsItemHovered(); // Hovered
        const bool isActive = ImGui::IsItemActive();   // Held
        const ImVec2 origin = { canvasP0.x + canvasSZ.x * 0.5f + m_scrolling.x, canvasP0.y + canvasSZ.y * 0.5f + m_scrolling.y};

        const auto toScreen = [&](const aiko::vec2& point) -> ImVec2
        {
            return ImVec2{ origin.x + point.x * PixelsPerUnit, origin.y - point.y * PixelsPerUnit};
        };

        const auto toProfile = [&](const ImVec2& point) -> aiko::vec2
        {
            return aiko::vec2{(point.x - origin.x) / PixelsPerUnit, (origin.y - point.y) / PixelsPerUnit};
        };

        const aiko::vec2 mousePosInCanvas = toProfile(io.MousePos); // Mouse position in profiler canvas

        const auto closest = GetClosestPointOnPolyline(m_editProfile, mousePosInCanvas);
        const auto hoveredControlPoint = isHovered ? FindClosestControlPoint(m_editProfile, mousePosInCanvas) : std::nullopt;

        // Add a point or drag control
        if (isHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            if (io.KeyCtrl && hoveredControlPoint)
            {
                if (*hoveredControlPoint == 0 && points.size() >= 3 && !isClosed)
                {
                    isClosed = true;
                }
                else
                {
                    m_draggedControlPoint = hoveredControlPoint;
                }
            }
            else if (!io.KeyCtrl)
            {
                if (!hoveredControlPoint && closest.distance < SnapDistance / PixelsPerUnit)
                {
                    const size_t insertionIndex = closest.segmentIndex + 1;
                    points.insert(points.begin() + insertionIndex, closest.position);
                }
                else if (!isClosed && !hoveredControlPoint)
                {
                    points.push_back(mousePosInCanvas);
                }
            }
        }

        if (m_draggedControlPoint && isActive && ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            points[*m_draggedControlPoint] = mousePosInCanvas;
        }
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left) == false)
        {
            m_draggedControlPoint.reset();
        }

        // Pan (we use a zero mouse threshold when there's no context menu)
        // You may decide to make that threshold dynamic based on whether the mouse is hovering something etc.
        const float mouse_threshold_for_pan = -1.0f;
        if (isActive && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
        {
            m_scrolling.x += io.MouseDelta.x;
            m_scrolling.y += io.MouseDelta.y;
        }

        // Context menu (under default mouse threshold)
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsMouseHoveringRect(canvasP0, canvasP1))
        {
            m_contextPointIndex = FindClosestControlPoint(m_editProfile, mousePosInCanvas);
        }
        ImGui::OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);

        if (ImGui::BeginPopup("context"))
        {
            if (m_contextPointIndex && *m_contextPointIndex < points.size())
            {
                ImGui::Text("Point %zu", *m_contextPointIndex);
                ImGui::Separator();

                if (ImGui::MenuItem("Delete Point"))
                {
                    points.erase(points.begin() + *m_contextPointIndex);

                    m_contextPointIndex.reset();
                    m_draggedControlPoint.reset();

                    if (points.size() < 3)
                    {
                        isClosed = false;
                    }
                }
            }
            ImGui::EndPopup();
        }

        // Draw grid + all lines in the canvas
        drawList->PushClipRect(canvasP0, canvasP1, true);
        constexpr float GridStep = 64.0f;
        for (float x = std::fmod(origin.x - canvasP0.x, GridStep); x < canvasSZ.x; x += GridStep)
        {
            drawList->AddLine({canvasP0.x + x, canvasP0.y}, {canvasP0.x + x, canvasP1.y}, GridColor);
        }
        for (float y = std::fmod(origin.y - canvasP0.y, GridStep); y < canvasSZ.y; y += GridStep)
        {
            drawList->AddLine({canvasP0.x, canvasP0.y + y}, {canvasP1.x, canvasP0.y + y}, GridColor);
        }

        if (isClosed && points.size() >= 3)
        {
            const ImVec2 first = toScreen(points.front());
            const ImVec2 last = toScreen(points.back());
            drawList->AddLine(first, last, PolylineColor, PolylineSegmentThickness);
        }

        if (points.size() >= 2)
        {
            for (size_t n = 1; n < points.size(); ++n)
            {
                const ImVec2 start = toScreen(points[n - 1]);
                const ImVec2 end = toScreen(points[n]);
                drawList->AddLine(start, end, PolylineColor, PolylineSegmentThickness);
            }
        }
        for (size_t n = 0; n < points.size(); ++n)
        {
            const bool hovered = hoveredControlPoint && *hoveredControlPoint == n;
            const bool dragged = m_draggedControlPoint && *m_draggedControlPoint == n;

            const ImVec2 position = toScreen(points[n]);

            ImU32 color = ControlPointColor;
            float radius = ControlPointSize;

            if (hovered)
            {
                color = HoveredPointColor;
                radius = 6.0f;
            }

            if (dragged)
            {
                color = DraggedPointColor;
                radius = 7.0f;
            }

            drawList->AddCircleFilled(position, radius + 2.0f, ControlPointOutline);
            drawList->AddCircleFilled(position, radius, color);
        }

        if (isHovered && !hoveredControlPoint && closest.distance < SnapDistance / PixelsPerUnit)
        {
            const ImVec2 position = toScreen(closest.position);
            drawList->AddCircle(position, 6.0f, SnapPreviewColor, 16, 2.0f);
        }

        drawList->PopClipRect();

        ImGui::End();

    }

    ImGuiRoadProfiler::PolylineProjection ImGuiRoadProfiler::GetClosestPointOnPolyline(const RoadProfile& roadProfiler, const aiko::vec2& mousePosInCanvas)
    {
        ImGuiRoadProfiler::PolylineProjection result = {};
        for (size_t i = 0; i + 1 < roadProfiler.points.size(); ++i)
        {
            EvaluateSegmentProjection(mousePosInCanvas, roadProfiler.points[i], roadProfiler.points[i + 1], i, result);
        }
        if (roadProfiler.closed && roadProfiler.points.size() >= 3)
        {
            EvaluateSegmentProjection(mousePosInCanvas, roadProfiler.points.back(), roadProfiler.points.front(), roadProfiler.points.size() - 1, result);
        }
        return result;
    }

    std::optional<size_t> ImGuiRoadProfiler::FindClosestControlPoint(const RoadProfile& roadProfiler, const aiko::vec2& mousePosInCanvas)
    {
        std::optional<size_t> closestIndex = std::nullopt;
        const float selectionRadius = SelectedNodeRadius / PixelsPerUnit;
        float minDistanceSquare = selectionRadius * selectionRadius;
        for (size_t i = 0 ; i < roadProfiler.points.size() ; ++i)
        {
            const float distanceSquared = aiko::math::distanceSquared(roadProfiler.points[i], mousePosInCanvas);
            if (distanceSquared < minDistanceSquare)
            {
                minDistanceSquare = distanceSquared;
                closestIndex = i;
            }
        }
        return closestIndex;
    }

    void ImGuiRoadProfiler::EvaluateSegmentProjection(const aiko::vec2& mousePosition, const aiko::vec2& pointA, const aiko::vec2& pointB, size_t segmentIdx, PolylineProjection& result)
    {

        const aiko::vec2 AB = pointB - pointA;
        const aiko::vec2 AP = mousePosition - pointA;

        const float denominator = aiko::math::dot(AB, AB);

        if (denominator <= epsilon)
        {
            return;
        }

        const float t = aiko::math::clamp(aiko::math::dot(AP, AB) / denominator, 0.0f, 1.0f);

        const aiko::vec2 Q = aiko::vec2(pointA.x + AB.x * t, pointA.y + AB.y * t);

        const auto distance = aiko::math::distance(Q, mousePosition);

        if (distance < result.distance)
        {
            result.distance = distance;
            result.position = Q;
            result.segmentIndex = segmentIdx;
        }

    }

}
