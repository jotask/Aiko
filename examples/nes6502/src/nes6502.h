#pragma once

#include <string>
#include <thread>

#include "application/application.h"
#include "models/shader.h"
#include "types/textures.h"
#include "types/asset_type.h"

#include "cpu.h"
#include "cartridge.h"
#include "bus.h"
#include "ppu.h"
#include "memory.h"

namespace nes
{

    class RenderSystem;

    class nes6502
    {
    public:
        nes6502();
        ~nes6502();
        void start();
        void stop();
        void init();
        void reset();
        void insertCartridge(const char*);

        std::size_t getNofCycles() const;

        nes::Bus* getBus();

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
