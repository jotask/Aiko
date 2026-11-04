#pragma once

#include "models/material.h"
#include "models/mesh.h"
#include "track_generator.h"
#include "track_navigation.h"
#include "world/road_profile.h"
#include "world/spline.h"

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

        Track();
        ~Track();

        void init(aiko::AssetContext&);

        bool generate(const TrackGeneratorSettings& settings);

        bool isOnRoad(const TrackNavigation::TrackProjection& point) const;
        bool isOnRoad(const aiko::vec3& point) const;
        TrackNavigation::TrackProjection projectPoint(const aiko::vec3& point) const;

        void render(aiko::RenderContext& context);

        FinishLine getFinishLine() const;
        float getTrackLength() const { return m_trackNavigation.getTrackLength(); }

        bool& isDebugEnabled() { return m_debugEnabled; }
        RoadProfile& roadProfile() { return m_profile; }

    private:

        void renderDebug(aiko::RenderContext& context);

        float m_trackWidth = 0.0f;

        Spline m_centerLine;
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