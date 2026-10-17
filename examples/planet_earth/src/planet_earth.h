#pragma once

#include "aiko_includes.h"
#include "layers/layer.h"

#include "data_request.h"
#include "data_parser.h"

#include <models/material.h>
#include <models/mesh.h>

#include <string>
#include <deque>

namespace aiko
{
    class CameraComponent;
}

namespace pe
{

    class PlanetEarth : public aiko::Layer
    {
    protected:
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;
    private:

        HeightMapResolution m_mapResolution = HeightMapResolution::R360x180;

        aiko::CameraComponent* m_camera;
        bool m_rotateCamera = true;

        MapDataRequest m_downloader;
        DataGenerator m_generator;
        bool m_showOcean = true;

        aiko::LightData m_sunLight;
        bool m_animateSun = true;
        float m_sunSpeed = 0.05f; // Radians per second

        aiko::Mesh m_oceanMesh;
        aiko::Material m_oceanMaterial;

        aiko::Material m_earthMaterial;
        std::deque<TerrainChunk> m_worldChunks;

    };

}

