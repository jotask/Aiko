#pragma once

#include "nes/nintendo_entertainment_system.h"
#include "emulator_window.h"

namespace nes
{
    class PpuWindow : public EmulatorWindow
    {
    public:
        PpuWindow(Naiko* n);
        virtual void update() override;
        virtual void render() override;

    private:

    };
}
