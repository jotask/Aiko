#pragma once

#include <string>
#include <thread>

#include "application/application.h"
#include "models/shader.h"
#include "types/textures.h"
#include "types/asset_type.h"

#include "cpu.h"
#include "bus.h"
#include "ppu.h"
#include "memory.h"

namespace nes
{

    class RenderSystem;

    class nes6502 : public aiko::Application
    {
    public:
        nes6502();
        ~nes6502();
    protected:
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;

        void reset();

    private:

        void onCycle();

        nes::Cpu cpu;
        nes::Memory memory;
        nes::Bus bus;
        nes::Ppu ppu;

        std::atomic<double> runClock;
        std::thread clock;

    };

}
