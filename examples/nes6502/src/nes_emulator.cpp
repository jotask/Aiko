#include "nes_emulator.h"

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

#include "nes/bus.h"
#include "nes/memory.h"
#include "nes/utils/nes_utils.h"
#include "nes/cpu/instructions.h"

#include "aiko_extensions/nes_emulator_component.h"

#include "nes/tests/online_test_manager.h"

#include <imgui.h>

namespace nes
{

    NesEmulator::NesEmulator()
        : aiko::Application(aiko::AikoConfig("NesEmulator", 1024, 768))
        , m_emulator(this, &m_nes)
    {

    }

    NesEmulator::~NesEmulator()
    {
    }

    void NesEmulator::init()
    {

        m_nesgo = createGameObject("NesTexture");
        m_nesgo->addComponent<NesComponent>();

        nes::test::online::TestManager::it().run();
        const std::string cartridge = AssetPath + "nestest.nes";
        m_nes.insertCartridge(cartridge.c_str());
        m_nes.reset();
        m_nes.start();
    }

    void NesEmulator::update()
    {

    }

    void NesEmulator::render()
    {
        static std::stringstream fmt;
        constexpr float space = 15.0f;
        float y = 0.0f;
        drawText("nes6502", 0.0f, space * y++);

        fmt.str("");    fmt << "Cycles: " << m_nes.getNofCycles();
        drawText(fmt.str(), 0.0f, space * y++);

        nes::Cpu* cpu = m_nes.getBus()->getMicroprocesor<Cpu>();

        fmt.str("");    fmt << "AddressingMode: " << nes::to_string(cpu->currentAddressMode());
        drawText(fmt.str(), 0.0f, space * y++);

        fmt.str("");    fmt << "Instruction: " << nes::to_string(cpu->currentInstruction());
        drawText(fmt.str(), 0.0f, space * y++);

    }

}
