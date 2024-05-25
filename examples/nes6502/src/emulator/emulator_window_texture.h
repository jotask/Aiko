#pragma once

#include "nes/nintendo_entertainment_system.h"
#include "emulator_window.h"

namespace nes
{
    class NesComponent;
    class GameWindow : public EmulatorWindow
    {
    public:
        GameWindow(Naiko* n, NesComponent* cmp);
        virtual void update() override;
        virtual void render() override;
    private:
        NesComponent* component;
    };
}
