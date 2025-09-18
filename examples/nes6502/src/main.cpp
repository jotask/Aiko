#include <stdlib.h>

#include "nes_emulator.h"

int main()
{
    nes::NesEmulator nes;
    nes.run();
    return EXIT_SUCCESS;
}