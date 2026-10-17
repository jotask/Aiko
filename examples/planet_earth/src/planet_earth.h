#pragma once

#include "aiko_includes.h"
#include "layers/layer.h"

#include "data_request.h"
#include "data_parser.h"

#include <models/material.h>
#include <models/mesh.h>

#include <string>

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

        MapDataRequest m_downloader;
        DataGenerator m_generator;

        aiko::Material m_defaultMaterial;
        aiko::Mesh m_worldMesh;

    };

}

