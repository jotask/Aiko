#pragma once

#include "models/material.h"
#include "models/mesh.h"
#include "track_generator.h"
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
        Track();
        ~Track();

        void init(aiko::AssetContext&);

        bool generate(const TrackGeneratorSettings& settings);

        void render(aiko::RenderContext& context);

        bool& isDebugEnabled() { return m_debugEnabled; }
        RoadProfile& roadProfile() { return m_profile; }

    private:

        void renderDebug(aiko::RenderContext& context);

        Spline m_centerLine;
        RoadProfile m_profile;

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