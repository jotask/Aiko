#pragma once

#include "nes/nintendo_entertainment_system.h"
#include "emulator_window.h"

namespace nes
{
    class GameWindow : public EmulatorWindow
    {
    public:
        GameWindow(Naiko* n);
        virtual void update() override;
        virtual void render() override;

    private:

    };
}
