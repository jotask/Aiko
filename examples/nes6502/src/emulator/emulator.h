#pragma once

#include <vector>

#include <aiko_includes.h>

#include "nes/nintendo_entertainment_system.h"
#include "emulator/emulator_window.h"

namespace nes
{
    class NesEmulator;
    class Cpu;
    class Naiko

    {
    public:
        Naiko(NesEmulator* em, Nes* nes);
        void update();
        void render();

        NesEmulator* getApplication() { return em; }
        Cpu* getCpu();
    private:
        NesEmulator* em;
        Nes* m_nes;
        std::vector<aiko::AikoUPtr<EmulatorWindow>> windows;
    };
}
