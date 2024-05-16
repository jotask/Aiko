#pragma once

#include  <string>
#include <iomanip>
#include <sstream>

#include "nes_types.h"

namespace nes
{

    static Byte toByte(std::string value)
    {
        std::stringstream ss;
        ss << std::hex << value;
        int temp;
        ss >> temp;
        Byte result = static_cast<Byte>(temp);
        return result;
    }

    static Word toWord(std::string value)
    {
        std::stringstream ss;
        ss << std::hex << value;
        int temp;
        ss >> temp;
        Word result = static_cast<Word>(temp);
        return result;
    }

    static std::string toString(Byte value)
    {
        static std::string buffer;
        static std::stringstream ss;
        ss.str("");
        ss << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(value);
        return ss.str();
    }

    static std::string toString(Word value)
    {
        static std::string buffer;
        static std::stringstream ss;
        ss.str("");
        ss << "0x" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(value);
        return ss.str();
    }

    inline Byte getHigh(Word value)
    {
        // Little endian
        return (value >> 8) & 0xFF;
    }

    inline Byte getLow(Word value)
    {
        // Little endian
        return value & 0xFF;
    }

    inline Word toWord(Byte high, Byte low)
    {
        // Little endian
        return (static_cast<Word>(low) | (static_cast<Word>(high) << 8));
    }

    inline Byte toByte(Word word)
    {
        Byte low = static_cast<Byte>(word & 0xFF);
        Byte high = static_cast<Byte>((word >> 8) & 0xFF);
        return high;
    }

}
