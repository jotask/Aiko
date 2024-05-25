#pragma once

#include "nes/nintendo_entertainment_system.h"
#include "emulator_window.h"

namespace nes
{
    class CpuWindow : public EmulatorWindow
    {
    public:
        CpuWindow(Naiko* n);
        virtual ~CpuWindow() = default;
        virtual void update() override;
        virtual void render() override;

    private:

    };
}
