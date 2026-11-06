#pragma once

#include "extensions/ImGuiRoadProfiler.h"
#include "simulation/vehicle.h"
#include "simulation/vehicle_agent.h"
#include "training/training_session.h"
#include "world/track.h"
#include "world/track_environment.h"

#include <layers/layer.h>
#include <models/model.h>

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

        struct TrackImguiSettings
        {
            bool isOpen = false;
            bool isRoadProfilerEditor = false;
        };

        struct VehicleDebugInfo
        {
            bool isOpen = false;
            bool renderSensors = false;
        };

        struct TrainingSessionInfo
        {
            bool isOpen = false;
            size_t regenerateTrackEveryNGenerations = 0;
            uint32_t nextTrackSeed = 0;
        };

        enum class DrivingMode
        {
            Reference,
            NEAT,
        };

        bool regenerateWorld();
        void resetAgents();
        bool resetTraining();

        void simulateStep(float dt);

        void renderImgui();
        void renderMenuBar();
        void renderTrackGenerationSettings();
        void renderVehicleDebug();
        void renderTrainingSimulation();

        static constexpr float MaxEpisodeDuration = 120.0f;
        static constexpr float StagnationTimeout = 8.0f;
        static constexpr float MinimumProgress = 0.5f;

        TrackGeneratorSettings m_trackSettings = {};
        Track m_track;
        TrackEnvironment m_environment;

        TrackImguiSettings m_trackImgui;
        ImGuiRoadProfiler m_profileEditor;
        VehicleDebugInfo m_vehicleImgui;
        TrainingSessionInfo m_trainingImgui;

        aiko::Model m_vehicleModel;

        int m_vehicleCount = 100;
        int m_selectedAgent = 0;

        DrivingMode m_drivingMode = DrivingMode::NEAT;
        bool m_trainingFailed = false;

        std::vector<VehicleAgent> m_agents;
        TrainingSession m_trainingSession;

        static constexpr float FixedTimeStep = 1.0f / 60.0f;

        float m_simulationAccumulator = 0.0f;
        bool m_autonomousDriving = true;
        bool m_referenceDriverVariation = true;

        float m_episodeElapsedTime = 0.0f;
        bool m_episodeFinished = false;

    };

}

