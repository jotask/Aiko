#include "emulator_ppu.h"

#include "emulator/emulator.h"
#include "nes_emulator.h"

#include <aiko_includes.h>
#include <imgui.h>

namespace nes
{
    PpuWindow::PpuWindow(Naiko* n)
        : EmulatorWindow(n, "Ppu")
    {

    }

    void PpuWindow::update()
    {

    }

    void PpuWindow::render()
    {
        if (ImGui::Begin(name.c_str(), &is_open))
        {

        }
        ImGui::End();
    }

}
