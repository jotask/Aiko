#include "nes_emulator.h"

#include <chrono>
#include <sstream>
#include <cassert>
#include <cmath>

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
#include "constants.h"

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
        : aiko::Application(aiko::AikoConfig("NesEmulator", 1024, 768, aiko::DARKGREEN))
        , m_emulator(this, &m_nes)
    {

    }

    NesEmulator::~NesEmulator()
    {
    }

    nes::NesComponent* NesEmulator::getNesGo() const
    {
        return m_nesgo;
    }

    aiko::PboTextureComponent* NesEmulator::getPT0() const
    {
        return pattern_table_0;
    }

    aiko::PboTextureComponent* NesEmulator::getPalette() const
    {
        return palette;
    }

    void NesEmulator::init()
    {

        auto go = createGameObject("NesTexture");
        m_nesgo = go->addComponent<NesComponent>().get();

        auto table_pattern_go_1 = createGameObject("CHR table");
        pattern_table_0 = table_pattern_go_1->addComponent<aiko::PboTextureComponent>("Pt0", 256, 128, false).get();

        auto palette_go = createGameObject("Palette");
        const Byte size = (Byte) std::sqrt(COLOUR_PALETTE_SIZE);
        palette = palette_go->addComponent<aiko::PboTextureComponent>("pal", size, size, false).get();

        m_emulator.init();

        nes::test::online::TestManager::it().run();
        const std::string cartridge = global::getAssetPath("nestest.nes");
        m_nes.insertCartridge(cartridge.c_str());
        m_nes.reset();
        m_nes.start();
    }

    void NesEmulator::update()
    {
        m_emulator.update();
    }

    void NesEmulator::render()
    {
        m_emulator.render();
    }

}
