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
            ImGui::Text("C : Z : I : D : B : U : V : N");
            ImGui::Text("%d : %d : %d : %d : %d : %d : %d : %d"
                , cpu->getFlag(Cpu::StatusFlags::C)
                , cpu->getFlag(Cpu::StatusFlags::Z)
                , cpu->getFlag(Cpu::StatusFlags::I)
                , cpu->getFlag(Cpu::StatusFlags::D)
                , cpu->getFlag(Cpu::StatusFlags::B)
                , cpu->getFlag(Cpu::StatusFlags::U)
                , cpu->getFlag(Cpu::StatusFlags::V)
                , cpu->getFlag(Cpu::StatusFlags::N)
            );


        }
        ImGui::End();
    }

}
