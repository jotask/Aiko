#pragma once

#include "aiko_macros.h"

#include <cstdint>
#include <span>

namespace wc
{

    inline uint8_t getBit(uint8_t byte, uint8_t bitIndex)
    {
        AIKO_ASSERT(bitIndex < 8, "Bit index must be between 0 and 7");
        return (byte >> bitIndex) & 1;
    }

    inline uint16_t readU16LE(std::span<const uint8_t> data, size_t offset)
    {
        AIKO_ASSERT(offset <= data.size() && data.size() - offset >= 2, "Not enough bytes to read uint16_t");
        return uint16_t(data[offset + 0]) | (uint16_t(data[offset + 1]) << 8);
    }

    inline uint32_t readU24LE(std::span<const uint8_t> data, size_t offset)
    {
        AIKO_ASSERT(offset <= data.size() && data.size() - offset >= 3, "Not enough bytes to read uint24_t");
        return uint32_t(data[offset + 0]) | (uint32_t(data[offset + 1]) << 8) | (uint32_t(data[offset + 2]) << 16);
    }

    inline uint32_t readU32LE(std::span<const uint8_t> data, size_t offset)
    {
        AIKO_ASSERT(offset <= data.size() && data.size() - offset >= 4, "Not enough bytes to read uint32_t");
        return uint32_t(data[offset + 0]) | (uint32_t(data[offset + 1]) << 8) | (uint32_t(data[offset + 2]) << 16) | (uint32_t(data[offset + 3]) << 24);
    }

    inline uint8_t getHighByte(uint16_t input)
    {
        return (input >> 8) & 0xff;
    }

    inline uint8_t getLowByte(uint16_t input)
    {
        return input & 0xff;
    }

    inline uint16_t getHighWord(size_t input)
    {
        return static_cast<uint16_t>((input >> 16) & 0xFFFF);
    }

    inline uint16_t getLowWord(size_t input)
    {
        return static_cast<uint16_t>(input & 0xFFFF);
    }

    inline size_t combineWords(uint16_t high, uint16_t low)
    {
        return (size_t(high) << 16) | low;
    }

    inline uint16_t wordByte(uint8_t high, uint8_t low)
    {
        return (uint16_t(high) << 8) | low;
    }
}