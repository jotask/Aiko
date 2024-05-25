#include "emulator.h"

#include "emulator/emulator_window_texture.h"
#include "emulator/emulator_cpu.h"
#include "emulator/emulator_ppu.h"

namespace nes
{
    Naiko::Naiko(NesEmulator* em, Nes* nes)
        : em(em)
        , m_nes(nes)
    {
        windows.emplace_back(std::make_unique<GameWindow>(this));
        windows.emplace_back(std::make_unique<CpuWindow>(this));
        windows.emplace_back(std::make_unique<PpuWindow>(this));
    }

    void Naiko::update()
    {
        for (auto& w : windows)
        {
            w->update();
        }
    }

    void Naiko::render()
    {
        for (auto& w : windows)
        {
            w->render();
        }
    }

    Cpu* Naiko::getCpu()
    {
        return m_nes->getBus()->getMicroprocesor<Cpu>();
    }

}
