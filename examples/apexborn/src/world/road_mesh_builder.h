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

        static aiko::MeshAsset build(const Spline& spline, const RoadProfile& profile, const RoadMeshBuilderDesc& desc);
    };

}