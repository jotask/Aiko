#pragma once

#include "nes/nintendo_entertainment_system.h"
#include "emulator_window.h"

namespace nes
{
    class CartridgeWindow : public EmulatorWindow
    {
    public:
        CartridgeWindow(Naiko* n);
        virtual void update() override;
        virtual void render() override;

    private:

        std::vector<aiko::Color> convertPatternTableToTexture(const std::vector<Byte>& patternTable);

    };
}
