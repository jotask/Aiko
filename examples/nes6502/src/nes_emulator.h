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

        aiko::SpriteComponent* getNesGo() const;
        aiko::SpriteComponent* getPT0() const;
        aiko::SpriteComponent* getPalette() const;

    protected:
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;

    private:

        Nes m_nes;
        Naiko m_emulator;

        aiko::SpriteComponent* m_nesgo;
        aiko::SpriteComponent* pattern_table_0;
        aiko::SpriteComponent* palette;

    };

}
