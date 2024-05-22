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
        clock = std::thread(&Nes::onCycle, this);
    }

    void Nes::stop()
    {
        runClock = false;
        if (std::thread::id() != clock.get_id())
        {
            clock.join();
        }
    }

    void Nes::reset()
    {
        bus.reset();
    }

    void Nes::onCycle()
    {
        using namespace std::literals;
        using clock_type = std::chrono::high_resolution_clock;

        constexpr auto wait_time = 1ms;// 1000ms;

        auto when_started = clock_type::now();
        auto target_time = when_started + wait_time;
        while (runClock)
        {
            ppu.clock();
            if (nOfcycles % 3 == 0)
            {
                cpu.clock();
            }
            nOfcycles++;
            std::this_thread::sleep_until(target_time);
            target_time += wait_time;
        }

    }

    void Nes::insertCartridge(const char* file)
    {
        cartridge.load(file);
    }

    std::size_t Nes::getNofCycles() const
    {
        return nOfcycles;;
    }

    nes::Bus* Nes::getBus()
    {
        return &bus;
    }

}
