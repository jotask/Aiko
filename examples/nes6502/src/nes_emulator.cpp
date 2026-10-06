#include "nes_emulator.h"

#include "constants.h"
#include "layers/contexts/render_context.h"
#include "layers/contexts/scene_context.h"
#include "nes/cpu/instructions.h"
#include "nes/nes_types.h"
#include "nes/tests/online_test_manager.h"
#include "nes/utils/nes_utils.h"
#include <layers/contexts/asset_context.h>
#include <layers/contexts/render_context.h>
#include <assets/types/texture_asset.h>
#include <models/game_object.h>

#include <imgui.h>

namespace nes
{

    namespace
    {
        aiko::TextureAsset createTextureAsset(int width, int height)
        {
            aiko::TextureAsset texture;

            texture.desc =
            {
                .type = aiko::TextureType::Sampled,
                .format = aiko::TextureFormat::RGBA8,
                .width = width,
                .height = height,
                .mipmaps = 1,
                .computeWrite = false,
            };

            texture.pixels.resize(static_cast<size_t>(width) * static_cast<size_t>(height), aiko::BLACK);

            return texture;
        }
    }

    NesEmulator::NesEmulator()
        : m_emulator(this, &m_nes)
    {
    }

    const aiko::AssetId& NesEmulator::getNesTextureId() const
    {
        return m_nesTextureId;
    }

    const aiko::AssetId& NesEmulator::getPatternTableTextureId() const
    {
        return m_patternTableTextureId;
    }

    const aiko::AssetId& NesEmulator::getPaletteTextureId() const
    {
        return m_paletteTextureId;
    }

    void NesEmulator::init()
    {

        auto* cam = scene().createCamera( aiko::camera::CameraController::Static, aiko::Camera::CameraType::Orthographic );
        cam->getCamera().position = { 0.0f, 1.0f, 3.0f };

        m_nesTextureId = assets().createTexture( createTextureAsset( static_cast<int>(NES_WIDTH), static_cast<int>(NES_HEIGHT) ) );

        m_patternTableTextureId = assets().createTexture( createTextureAsset(256, 128) );

        constexpr int paletteWidth = static_cast<int>(COLOUR_PALETTE_SIZE / 4);

        constexpr int paletteHeight = static_cast<int>(COLOUR_PALETTE_SIZE / 16);

        m_paletteTextureId = assets().createTexture( createTextureAsset( paletteWidth, paletteHeight ) );

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

    aiko::ImguiTextureId NesEmulator::getImguiTextureId(const aiko::AssetId& textureId) const
    {
        const aiko::SamplerState sampler
        {
            .minFilter = aiko::TextureFilter::Nearest,
            .magFilter = aiko::TextureFilter::Nearest,
            .mipFilter = aiko::TextureMipFilter::None,
            .wrapU = aiko::TextureWrapMode::Clamp,
            .wrapV = aiko::TextureWrapMode::Clamp,
        };
        return renderer().getTextureId(textureId, sampler);
    }

    void NesEmulator::updateNesTexture(const std::vector<aiko::Color>& pixels)
    {
        aiko::TextureAsset& texture = assets().getMutableTexture(m_nesTextureId);

        AIKO_ASSERT(texture.pixels.size() == pixels.size(), "NES framebuffer pixel count mismatch");

        texture.pixels = pixels;
        assets().invalidateTexture(m_nesTextureId);
    }

    void NesEmulator::updatePatternTableTexture(const std::vector<aiko::Color>& pixels)
    {
        aiko::TextureAsset& texture = assets().getMutableTexture(m_patternTableTextureId);

        AIKO_ASSERT(texture.pixels.size() == pixels.size(), "NES pattern table pixel count mismatch");

        texture.pixels = pixels;
        assets().invalidateTexture(m_patternTableTextureId);
    }

    void NesEmulator::updatePaletteTexture(const std::vector<aiko::Color>& pixels)
    {
        aiko::TextureAsset& texture = assets().getMutableTexture(m_paletteTextureId);

        AIKO_ASSERT(texture.pixels.size() == pixels.size(), "NES palette pixel count mismatch");

        texture.pixels = pixels;
        assets().invalidateTexture(m_paletteTextureId);
    }

}
