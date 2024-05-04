#pragma once

#include <string>
#include <thread>

#include "application/application.h"
#include "models/shader.h"
#include "types/textures.h"
#include "types/asset_type.h"

#include "nes6502.h"

namespace nes
{

    class RenderSystem;

    class NesEmulator : public aiko::Application
    {
    public:
        NesEmulator();
        ~NesEmulator();
    protected:
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;

    private:

        nes::nes6502 m_nes;

    };

}
