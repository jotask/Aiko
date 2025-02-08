#pragma once

#include "application/application.h"

#include "loop_recorder.h"
#include "loop_visualizer.h"

namespace aiko::recorder
{

    class LoopRecorderGui : public Application
    {
    protected:
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;
    private:
        LoopRecorder m_loopRecorder;
        LoopVisualizer m_loopVisualizer;
    };

}

