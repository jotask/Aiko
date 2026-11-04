#pragma once

#include "models/material.h"
#include "models/mesh.h"
#include "track_generator.h"
#include "track_navigation.h"
#include "world/road_profile.h"
#include "world/spline.h"

#include <vector>
#include <cstddef>
#include <optional>

namespace aiko
{
    class AssetContext;
}

namespace ab
{

    class Track
    {
    public:

        struct FinishLine
        {
            aiko::vec3 position;
            aiko::vec3 forward;
        };

        struct RoadRaycastHit
        {
            aiko::vec3 position;
            float distance = 0.0f;
        };

        Track();
        ~Track();

        void init(aiko::AssetContext&);

        bool generate(const TrackGeneratorSettings& settings);

        std::optional<RoadRaycastHit> raycastBoundary(const aiko::vec3& origin, const aiko::vec3& direction, float maxDistance) const;

        bool isOnRoad(const TrackNavigation::TrackProjection& point) const;
        bool isOnRoad(const aiko::vec3& point) const;
        TrackNavigation::TrackProjection projectPoint(const aiko::vec3& point) const;

        void render(aiko::RenderContext& context);

        FinishLine getFinishLine() const;
        aiko::vec3 pointAtDistance(float distance) const { return m_trackNavigation.pointAtDistance(distance); }
        float getTrackLength() const { return m_trackNavigation.getTrackLength(); }
        float getTrackWidth() const { return m_trackWidth; }

        bool& isDebugEnabled() { return m_debugEnabled; }
        RoadProfile& roadProfile() { return m_profile; }

    private:

        struct BoundarySegment
        {
            aiko::vec3 start;
            aiko::vec3 end;
        };

        struct BoundaryGrid
        {
            aiko::vec2 origin = {};

            float cellSize = 2.0f;

            int columns = 0;
            int rows = 0;

            std::vector<BoundarySegment> segments;
            std::vector<std::vector<size_t>> cells;
        };

        void rebuildBoundaryGrid();

        void renderDebug(aiko::RenderContext& context);

        float m_trackWidth = 0.0f;

        BoundaryGrid m_boundaryGrid;

        Spline m_centerLine;
        Spline::Points m_leftBoundary;
        Spline::Points m_rightBoundary;

        RoadProfile m_profile;
        TrackNavigation m_trackNavigation;

        aiko::Mesh m_roadMesh;
        aiko::Material m_roadMaterial;

        aiko::Mesh m_edgeMarkingsMesh;
        aiko::Material m_edgeMarkingsMaterial;

        aiko::Mesh m_curbMesh;
        aiko::Material m_curbMaterial;

        TrackGenerator::TrackGenerationDebug m_generationDebug;

        bool m_debugEnabled = false;


    };

}