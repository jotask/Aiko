#include <stdlib.h>

#include "nes_emulator.h"

int main()
{
    aiko::Application app;
    app.pushLayer(std::make_unique<nes::NesEmulator>());
    app.run();
    return EXIT_SUCCESS;
}