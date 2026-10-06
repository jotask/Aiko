#include "nes_emulator.h"

#include "constants.h"
#include "layers/contexts/render_context.h"
#include "layers/contexts/scene_context.h"
#include "nes/cpu/instructions.h"
#include "nes/nes_types.h"
#include "nes/tests/online_test_manager.h"
#include "nes/utils/nes_utils.h"

#include <assets/types/texture_asset.h>
#include <models/game_object.h>

#include <imgui.h>

namespace nes
{
    NesEmulator::NesEmulator()
        : m_emulator(this, &m_nes)
    {
    }

    aiko::SpriteComponent* NesEmulator::getNesGo() const
    {
        return m_nesgo;
    }

    aiko::SpriteComponent* NesEmulator::getPT0() const
    {
        return pattern_table_0;
    }

    aiko::SpriteComponent* NesEmulator::getPalette() const
    {
        return palette;
    }

    void NesEmulator::init()
    {

        auto* cam = scene().createCamera( aiko::camera::CameraController::Static, aiko::Camera::CameraType::Orthographic );
        cam->getCamera().position = { 0.0f, 1.0f, 3.0f };

        auto setTextureConfiguration = [](aiko::Material& material)
        {
            material.m_lit = false;
            aiko::SamplerState sampler;
            sampler.minFilter = aiko::TextureFilter::Nearest;
            sampler.magFilter = aiko::TextureFilter::Nearest;
            sampler.mipFilter = aiko::TextureMipFilter::None;
            sampler.wrapU = aiko::TextureWrapMode::Clamp;
            sampler.wrapV = aiko::TextureWrapMode::Clamp;
            material.setTextureSampler("u_texture", sampler);
        };

        auto go = Instantiate("NesTexture");
        m_nesgo = go->addComponent<aiko::SpriteComponent>();
        aiko::TextureAsset nesTexture;
        nesTexture.desc =
        {
            .type = aiko::TextureType::Sampled,
            .format = aiko::TextureFormat::RGBA8,
            .width = static_cast<int>(NES_WIDTH),
            .height = static_cast<int>(NES_HEIGHT),
            .mipmaps = 1,
            .computeWrite = false,
        };
        nesTexture.pixels.resize(
            static_cast<size_t>(NES_WIDTH) *
            static_cast<size_t>(NES_HEIGHT),
            aiko::BLACK
        );

        m_nesgo->load(std::move(nesTexture));
        setTextureConfiguration(m_nesgo->getMaterial());

        auto table_pattern_go_1 = Instantiate("CHR table");
        pattern_table_0 = table_pattern_go_1->addComponent<aiko::SpriteComponent>();
        aiko::TextureAsset patternTexture;
        patternTexture.desc =
        {
            .type = aiko::TextureType::Sampled,
            .format = aiko::TextureFormat::RGBA8,
            .width = 256,
            .height = 128,
            .mipmaps = 1,
            .computeWrite = false,
        };
        patternTexture.pixels.resize(
            static_cast<size_t>(256) *
            static_cast<size_t>(128),
            aiko::BLACK
        );
        pattern_table_0->load(std::move(patternTexture));
        setTextureConfiguration(pattern_table_0->getMaterial());
        setTextureConfiguration(pattern_table_0->getMaterial());

        auto palette_go = Instantiate("Palette");
        constexpr const Byte palette_width = COLOUR_PALETTE_SIZE / 4;
        constexpr const Byte palette_height = COLOUR_PALETTE_SIZE / 16;
        palette = palette_go->addComponent<aiko::SpriteComponent>();
        aiko::TextureAsset paletteTexture;
        paletteTexture.desc =
        {
            .type = aiko::TextureType::Sampled,
            .format = aiko::TextureFormat::RGBA8,
            .width = static_cast<int>(palette_width),
            .height = static_cast<int>(palette_height),
            .mipmaps = 1,
            .computeWrite = false,
        };
        paletteTexture.pixels.resize(
            static_cast<size_t>(palette_width) *
            static_cast<size_t>(palette_height),
            aiko::BLACK
        );
        palette->load(std::move(paletteTexture));
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

    aiko::ImguiTextureId NesEmulator::getImguiTextureId(const aiko::SpriteComponent& sprite) const
    {
        return renderer().getTextureId(sprite.getTextureId());
    }

}
