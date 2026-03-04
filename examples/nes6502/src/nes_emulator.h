#pragma once

#include <aiko_types.h>
#include <aiko_includes.h>

#include "application/application.h"

#include "nes/nintendo_entertainment_system.h"
#include "emulator/emulator.h"

namespace aiko
{
    class TextureComponent;
}

namespace nes
{
    class NesComponent;
    class RenderSystem;

    class NesEmulator : public aiko::Layer
    {
    public:
        NesEmulator();
        virtual ~NesEmulator() = default;

        // aiko::texture::RenderTexture2D* getTargetTexture() const { return Application::getTargetTexture(); }

        aiko::AikoPtr<aiko::TextureComponent> getNesGo() const;
        aiko::AikoPtr<aiko::TextureComponent> getPT0() const;
        aiko::AikoPtr<aiko::TextureComponent> getPalette() const;

    protected:
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;

    private:

        Nes m_nes;
        Naiko m_emulator;

        aiko::AikoPtr<aiko::TextureComponent> m_nesgo;
        aiko::AikoPtr<aiko::TextureComponent> pattern_table_0;
        aiko::AikoPtr<aiko::TextureComponent> palette;

    };

}
