#include "emulator_cpu.h"

#include "emulator/emulator.h"
#include "nes_emulator.h"
#include "emulator.h"

#include "nes/cpu/cpu.h"
#include "nes/cpu/instructions.h"
#include "nes/utils/nes_utils.h"

#include <aiko_includes.h>
#include <imgui.h>

namespace nes
{
    CpuWindow::CpuWindow(Naiko* n)
        : EmulatorWindow(n, "Cpu")
    {

    }

    void CpuWindow::update()
    {

    }

    void CpuWindow::render()
    {
        if (ImGui::Begin(name.c_str(), &is_open))
        {
            Cpu* cpu = naiko->getCpu();
            ImGui::Text("Cycles : %u", cpu->getCycles());
            ImGui::Spacing();
            ImGui::Text("Instruction : %s", to_string(cpu->currentInstruction()));
            ImGui::Text("AddressingMode : %s", to_string(cpu->currentAddressMode()));
            ImGui::Spacing();
            ImGui::Text("PC : %s", toString(cpu->getPC()).c_str());
            ImGui::Text("SP : %s", toString(cpu->getSP()).c_str());
            ImGui::Spacing();
            ImGui::Text("A : %s", toString(cpu->getA()).c_str());
            ImGui::Text("X : %s", toString(cpu->getX()).c_str());
            ImGui::Text("Y : %s", toString(cpu->getY()).c_str());
            ImGui::Spacing();
            ImGui::Text("P : %s", toString(cpu->getP()).c_str());
            ImGui::Spacing();
            ImGui::Text("C : %d", cpu->getFlag(Cpu::StatusFlags::C));
            ImGui::Text("Z : %d", cpu->getFlag(Cpu::StatusFlags::Z));
            ImGui::Text("I : %d", cpu->getFlag(Cpu::StatusFlags::I));
            ImGui::Text("D : %d", cpu->getFlag(Cpu::StatusFlags::D));
            ImGui::Text("B : %d", cpu->getFlag(Cpu::StatusFlags::B));
            ImGui::Text("U : %d", cpu->getFlag(Cpu::StatusFlags::U));
            ImGui::Text("V : %d", cpu->getFlag(Cpu::StatusFlags::V));
            ImGui::Text("N : %d", cpu->getFlag(Cpu::StatusFlags::N));

        }
        ImGui::End();
    }

}
