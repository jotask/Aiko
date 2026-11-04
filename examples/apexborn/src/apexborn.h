#pragma once

#include "extensions/ImGuiRoadProfiler.h"
#include "simulation/vehicle.h"
#include "world/track.h"
#include "world/track_environment.h"

#include <layers/layer.h>

#include <aiko_includes.h>

namespace ab
{

    class ApexBorn : public aiko::Layer
    {
    public:
        ApexBorn();
        ~ApexBorn();

    protected:

        virtual void init() override;
        virtual void update() override;
        virtual void render() override;
    private:

        void loadVehicleModel();

        bool regenerateWorld();

        void renderImgui();
        void renderMenuBar();
        void renderTrackGenerationSettings();
        void renderVehicleDebug();

        struct TrackImguiSettings
        {
            bool isOpen = false;
            bool isRoadProfilerEditor = false;
        };

        TrackGeneratorSettings m_trackSettings = {};
        Track m_track;
        TrackEnvironment m_environment;
        TrackImguiSettings m_trackImgui;
        ImGuiRoadProfiler m_profileEditor;

        struct VehicleVisualPart
        {
            std::unique_ptr<aiko::Mesh> mesh;
            aiko::Material material;
        };
        std::vector<VehicleVisualPart> m_vehicleParts;

        static constexpr size_t N_Vehicles = 1;
        std::vector<Vehicle> m_vehicles;

    };

}

