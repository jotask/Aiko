#include "emulator.h"

#include "emulator/emulator_game.h"
#include "emulator/emulator_cpu.h"
#include "emulator/emulator_ppu.h"
#include "emulator/emulator_cartridge.h"
#include "nes/cartridge/cartridge.h"
#include "nes_emulator.h"

#include <aiko_includes.h>
#include <imgui.h>

namespace nes
{
    Naiko::Naiko(NesEmulator* em, Nes* nes)
        : em(em)
        , m_nes(nes)
    {
    }

    void Naiko::init()
    {
        windows.emplace_back(std::make_unique<GameWindow>(this));
        windows.emplace_back(std::make_unique<CpuWindow>(this));
        windows.emplace_back(std::make_unique<PpuWindow>(this));
        windows.emplace_back(std::make_unique<CartridgeWindow>(this));
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
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Nes"))
            {
                for (auto& w : windows)
                {
                    if (ImGui::MenuItem(w->getName().c_str(), "", w->getIsOpen()))
                    {
                        w->toggle();
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
        for (auto& w : windows)
        {
            if (w->getIsOpen())
            {
                w->render();
            }
        }
    }

    Cpu* Naiko::getCpu()
    {
        return m_nes->getBus()->getMicroprocesor<Cpu>();
    }

    Ppu* Naiko::getPpu()
    {
        return m_nes->getBus()->getMicroprocesor<Ppu>();
    }

    Cartridge* Naiko::getCartridge()
    {
        return m_nes->getBus()->getMicroprocesor<Cartridge>();
    }

}
