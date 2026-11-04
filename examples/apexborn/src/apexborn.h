#pragma once

#include "extensions/ImGuiRoadProfiler.h"
#include "simulation/vehicle.h"
#include "simulation/vehicle_agent.h"
#include "world/track.h"
#include <models/model.h>
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

        bool regenerateWorld();
        void resetAgents();

        void simulateStep(float dt);

        void renderImgui();
        void renderMenuBar();
        void renderTrackGenerationSettings();
        void renderVehicleDebug();

        struct TrackImguiSettings
        {
            bool isOpen = false;
            bool isRoadProfilerEditor = false;
            bool openVehicleEditor = false;
        };

        TrackGeneratorSettings m_trackSettings = {};
        Track m_track;
        TrackEnvironment m_environment;
        TrackImguiSettings m_trackImgui;
        ImGuiRoadProfiler m_profileEditor;

        aiko::Model m_vehicleModel;

        int m_vehicleCount = 12;
        int m_selectedAgent = 0;

        std::vector<VehicleAgent> m_agents;

        static constexpr float FixedTimeStep = 1.0f / 60.0f;

        float m_simulationAccumulator = 0.0f;
        bool m_autonomousDriving = true;

    };

}

