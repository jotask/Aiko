#pragma once

#include  <string>
#include <iomanip>
#include <sstream>

#include "nes_types.h"

namespace nes
{
    static std::string toString(Byte value)
    {
        static std::string buffer;
        static std::stringstream ss;
        ss.str("");
        ss << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(value);
        return ss.str();
    }

    inline Byte getHigh(Word value)
    {
        return (value >> 8) & 0xFF;
    }

    inline Byte getLow(Word value)
    {
        return value & 0xFF;
    }

}
