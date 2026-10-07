#pragma once

#include "emulator/emulator.h"
#include "imgui/aiko_imgui.h"
#include "nes/nintendo_entertainment_system.h"

#include <layers/layer.h>

#include <aiko_includes.h>
#include <aiko_types.h>

namespace nes
{
    class NesComponent;
    class RenderSystem;

    class NesEmulator : public aiko::Layer
    {
    public:
        NesEmulator();
        virtual ~NesEmulator() = default;

        const aiko::AssetId& getNesTextureId() const;
        const aiko::AssetId& getPatternTableTextureId() const;
        const aiko::AssetId& getPaletteTextureId() const;

        void updateNesTexture(const std::vector<aiko::Color>& pixels);
        void updatePatternTableTexture(const std::vector<aiko::Color>& pixels);
        void updatePaletteTexture(const std::vector<aiko::Color>& pixels);

        aiko::ImguiTextureId getImguiTextureId(const aiko::AssetId& textureId) const;

    protected:
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;

    private:

        Nes m_nes;
        Naiko m_emulator;

        aiko::AssetId m_nesTextureId = aiko::InvalidAssetId;
        aiko::AssetId m_patternTableTextureId = aiko::InvalidAssetId;
        aiko::AssetId m_paletteTextureId = aiko::InvalidAssetId;

    };

}
