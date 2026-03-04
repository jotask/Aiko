#include "nes_emulator.h"

#include <models/game_object.h>

#include "constants.h"
#include "nes/utils/nes_utils.h"
#include "nes/cpu/instructions.h"
#include "nes/tests/online_test_manager.h"
#include "nes/nes_types.h"

#include <imgui.h>

namespace nes
{
    NesEmulator::NesEmulator()
        : m_emulator(this, &m_nes)
    {
    }

    aiko::AikoPtr<aiko::TextureComponent> NesEmulator::getNesGo() const
    {
        return m_nesgo;
    }

    aiko::AikoPtr<aiko::TextureComponent> NesEmulator::getPT0() const
    {
        return pattern_table_0;
    }

    aiko::AikoPtr<aiko::TextureComponent> NesEmulator::getPalette() const
    {
        return palette;
    }

    void NesEmulator::init()
    {

        auto go = app->Instantiate("NesTexture");
        m_nesgo = go->addComponent<aiko::TextureComponent>();
        m_nesgo->getTexture().create(NES_WIDTH, NES_HEIGHT);

        auto table_pattern_go_1 = app->Instantiate("CHR table");
        pattern_table_0 = table_pattern_go_1->addComponent<aiko::TextureComponent>();
        pattern_table_0->getTexture().create(256, 128);

        auto palette_go = app->Instantiate("Palette");
        constexpr const Byte palette_width = COLOUR_PALETTE_SIZE / 4;
        constexpr const Byte palette_height = COLOUR_PALETTE_SIZE / 16;
        palette = palette_go->addComponent<aiko::TextureComponent>();
        palette->getTexture().create(palette_width, palette_height);

        m_emulator.init();
        if constexpr (NES_TESTS_ENABLED)
        {
            nes::test::online::TestManager::it().run();
        }
        const aiko::string cartridge = global::getAssetPath(NES_ROM);
        m_nes.insertCartridge(cartridge.c_str());
        m_nes.reset();
        m_nes.start();
    }

    void NesEmulator::update()
    {
        m_nes.update();
        m_emulator.update();
    }

    void NesEmulator::render()
    {
        m_emulator.render();
    }

}
