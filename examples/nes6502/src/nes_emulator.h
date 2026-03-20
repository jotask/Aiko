#pragma once

#include <aiko_types.h>
#include <aiko_includes.h>

#include "application/application.h"

#include "nes/nintendo_entertainment_system.h"
#include "emulator/emulator.h"

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

        aiko::AikoPtr<aiko::SpriteComponent> getNesGo() const;
        aiko::AikoPtr<aiko::SpriteComponent> getPT0() const;
        aiko::AikoPtr<aiko::SpriteComponent> getPalette() const;

    protected:
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;

    private:

        Nes m_nes;
        Naiko m_emulator;

        aiko::AikoPtr<aiko::SpriteComponent> m_nesgo;
        aiko::AikoPtr<aiko::SpriteComponent> pattern_table_0;
        aiko::AikoPtr<aiko::SpriteComponent> palette;

    };

}
