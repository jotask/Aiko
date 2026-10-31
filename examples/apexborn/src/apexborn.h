#pragma once

#include "extensions/ImGuiRoadProfiler.h"
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

        bool regenerateWorld();

        void renderImgui();
        void renderMenuBar();
        void renderTrackGenerationSettings();

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

    };

}

