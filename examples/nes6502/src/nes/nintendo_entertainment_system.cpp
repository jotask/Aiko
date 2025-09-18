#include "nintendo_entertainment_system.h"

#include <chrono>
#include <sstream>
#include <cassert>

#include "shared/math.h"
#include "models/game_object.h"
#include "components/camera_component.h"
#include "components/mesh_component.h"
#include "components/light_component.h"
#include "systems/render_system.h"
#include "core/utils.h"
#include "types/inputs.h"
#include "models/camera.h"
#include "shared/math.h"
#include "core/log.h"

#include "aiko_extensions/nes_events.h"

#include <aiko_includes.h>

namespace nes
{
    Nes::Nes()
        : runClock(true)
        , nOfcycles(0)
    {
        bus.m_nes = this;
        bus.addMicroprocesor(&cartridge);
        bus.addMicroprocesor(&cpu);
        bus.addMicroprocesor(&memory);
        bus.addMicroprocesor(&ppu);
    }

    Nes::~Nes()
    {
        stop();
    }

    void Nes::start()
    {
        runClock = true;
    }

    void Nes::stop()
    {
        runClock = false;
    }

    void Nes::reset()
    {
        bus.reset();
    }

    void Nes::onCycle()
    {
        ppu.clock();
        if (nOfcycles % 3 == 0)
        {
            cpu.clock();
        }
        nOfcycles++;
        aiko::EventSystem::it().sendEvent(NesOnClockEvent());
    }

    void Nes::insertCartridge(const char* file)
    {
        cartridge.load(file);
    }

    std::size_t Nes::getNofCycles() const
    {
        return nOfcycles;
    }

    nes::Bus* Nes::getBus()
    {
        return &bus;
    }

    void Nes::update()
    {
        static float fResidualTime = 0.0f;
        float fElapsedTime = aiko::Time::it().getDeltaTime();
        if (fResidualTime > 0.0f)
        {
            fResidualTime -= fElapsedTime;
        }
        else
        {
            fResidualTime += (1.0f / 60.0f) - fElapsedTime;
            do
            {
                onCycle();
            }
            while(ppu.frame_complete == false);
            // Disable now as it gets stuck
            // ppu.frame_complete = false;
        }
    }

}
