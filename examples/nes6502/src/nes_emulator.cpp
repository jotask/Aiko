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

#include "tests/test_manager.h"

#define NES_ENABLE_TESTS

namespace nes
{

    NesEmulator::NesEmulator()
    {

    }

    NesEmulator::~NesEmulator()
    {
    }

    void NesEmulator::init()
    {

#ifdef NES_ENABLE_TESTS
        test::TestManager manager;
        manager.init();
        manager.runAll();
#endif

        m_nes.reset();
        m_nes.init();

        m_nes.insertCartridge("C:/Users/j.iznardo/Documents/Aiko/examples/nes6502/assets/6502_functional_test.bin");
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
    }

}
