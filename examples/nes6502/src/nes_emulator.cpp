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

    aiko::AikoPtr<aiko::SpriteComponent> NesEmulator::getNesGo() const
    {
        return m_nesgo;
    }

    aiko::AikoPtr<aiko::SpriteComponent> NesEmulator::getPT0() const
    {
        return pattern_table_0;
    }

    aiko::AikoPtr<aiko::SpriteComponent> NesEmulator::getPalette() const
    {
        return palette;
    }

    void NesEmulator::init()
    {

        auto setTextureConfiguration = [](aiko::Material& material)
        {
            material.m_lit = false;
            material.m_diffuse.setTextureFilter(aiko::texture::TextureFilter::Nearest, aiko::texture::TextureFilter::Nearest);
            material.m_diffuse.setTextureMipFilter(aiko::texture::TextureMipFilter::None);
            material.m_diffuse.setTextureWrapMode(aiko::texture::TextureWrapMode::Clamp, aiko::texture::TextureWrapMode::Clamp);
        };

        auto camera = app->Instantiate("Camera");
        auto cam = camera->addComponent<aiko::CameraComponent>(aiko::camera::CameraController::Static, aiko::Camera::CameraType::Orthographic );
        camera->transform().position = { 0.0f, 1.0f, 3.0f };
        cam->getCamera().position = camera->transform().position;

        auto go = app->Instantiate("NesTexture");
        m_nesgo = go->addComponent<aiko::SpriteComponent>(NES_WIDTH, NES_HEIGHT);
        setTextureConfiguration(m_nesgo->getMaterial());

        auto table_pattern_go_1 = app->Instantiate("CHR table");
        pattern_table_0 = table_pattern_go_1->addComponent<aiko::SpriteComponent>(256, 128);
        setTextureConfiguration(pattern_table_0->getMaterial());

        auto palette_go = app->Instantiate("Palette");
        constexpr const Byte palette_width = COLOUR_PALETTE_SIZE / 4;
        constexpr const Byte palette_height = COLOUR_PALETTE_SIZE / 16;
        palette = palette_go->addComponent<aiko::SpriteComponent>(palette_width, palette_height);
        setTextureConfiguration(palette->getMaterial());

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
