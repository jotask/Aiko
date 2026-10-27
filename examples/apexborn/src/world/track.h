#pragma once

#include "models/material.h"
#include "models/mesh.h"
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

        void generate(aiko::AssetContext&);

        void render(aiko::RenderContext& context);

    private:

        Spline m_centerLine;

        float m_trackWidth = 1.0f;

        Spline::Points m_leftEdge;
        Spline::Points m_rightEdge;

        RoadProfile m_profile;

        aiko::Mesh m_roadMesh;
        aiko::Material m_roadMaterial;

    };

}