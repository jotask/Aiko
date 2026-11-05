#pragma once
#include "assets/types/mesh_asset.h"
#include "road_profile.h"
#include "spline.h"

namespace ab
{

    class RoadMeshBuilder
    {
    public:

        struct RoadMeshBuilderDesc
        {
            float roadWidth = 1.0f;
            aiko::vec3 worldUp = {0.0f, 1.0f, 0.0f};
        };

        struct RoadBuildResult
        {
            aiko::MeshAsset mesh;
            Spline::Points leftBoundary;
            Spline::Points rightBoundary;
        };

        static RoadBuildResult build(const Spline& spline, const RoadProfile& profile, const RoadMeshBuilderDesc& desc);
        static aiko::MeshAsset buildEdgeMarkings(const Spline& spline, const RoadProfile& profile, const RoadMeshBuilderDesc& descs);
        static aiko::MeshAsset buildCurbs(const Spline& spline, const RoadProfile& profile, const RoadMeshBuilderDesc& desc);
        static float getProfileTopHeight(const RoadProfile& profile, float x);
    };

}