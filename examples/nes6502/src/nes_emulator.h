#pragma once

#include <string>
#include <thread>

#include <aiko_includes.h>

#include "application/application.h"
#include "models/shader.h"
#include "types/textures.h"
#include "types/asset_type.h"

#include "nes/nintendo_entertainment_system.h"
#include "emulator/emulator.h"

namespace nes
{
    class NesComponent;
    class RenderSystem;

    class NesEmulator : public aiko::Application
    {
    public:
        NesEmulator();
        ~NesEmulator();

        aiko::texture::RenderTexture2D* getTargetTexture() const { return Application::getTargetTexture(); }

        aiko::PboTextureComponent* getNesGo() const;
        aiko::PboTextureComponent* getPT0() const;
        aiko::PboTextureComponent* getPalette() const;

    protected:
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;

    private:

        nes::Nes m_nes;
        nes::Naiko m_emulator;

        aiko::PboTextureComponent* m_nesgo;
        aiko::PboTextureComponent* pattern_table_0;
        aiko::PboTextureComponent* palette;

    };

}
