#include "nes/utils/nes_utils.h"

#include  <string>
#include <iomanip>
#include <sstream>

#include <aiko_includes.h>

#include "nes/cpu/cpu.h"
#include "nes/nes_types.h"

namespace nes
{

    void printStatusFlags(aiko::string prefix, Byte statusFlags)
    {

        auto getFlag = [](Byte P, Cpu::StatusFlags p)
        {
            return ((P & p) > 0) ? 1 : 0;
        };

        auto printP = [&](Byte b, const char* prefix)
            {
                
                aiko::Log::error(
                    prefix,
                    "StatusFlags::",
                    " C: ", unsigned(getFlag(b, Cpu::C)),
                    " Z: ", unsigned(getFlag(b, Cpu::Z)),
                    " I: ", unsigned(getFlag(b, Cpu::I)),
                    " D: ", unsigned(getFlag(b, Cpu::D)),
                    " B: ", unsigned(getFlag(b, Cpu::B)),
                    " U: ", unsigned(getFlag(b, Cpu::U)),
                    " V: ", unsigned(getFlag(b, Cpu::V)),
                    " N: ", unsigned(getFlag(b, Cpu::N))
                );
            };
    }

}
