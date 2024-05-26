#pragma once

#include <string>
#include <thread>

#include "application/application.h"
#include "models/shader.h"
#include "types/textures.h"
#include "types/asset_type.h"

#include "nes/cpu/cpu.h"
#include "nes/cartridge/cartridge.h"
#include "nes/bus.h"
#include "nes/ppu/ppu.h"
#include "nes/memory.h"

namespace nes
{

    class RenderSystem;

    class Nes
    {
    public:
        Nes();
        ~Nes();
        void start();
        void stop();
        void reset();
        void insertCartridge(const char*);

        std::size_t getNofCycles() const;

        nes::Bus* getBus();

        void update();

    // private:

        void onCycle();

        nes::Cartridge cartridge;
        nes::Cpu cpu;
        nes::Memory memory;
        nes::Bus bus;
        nes::Ppu ppu;

        std::atomic<double> runClock;
        std::thread clock;

        std::size_t nOfcycles;

    };

}
