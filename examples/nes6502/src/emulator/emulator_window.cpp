#include "emulator_window.h"

namespace nes
{
    EmulatorWindow::EmulatorWindow(Naiko* n, aiko::string str)
        : naiko(n)
        , name(str)
        , is_open(true)
    {

    }
}
