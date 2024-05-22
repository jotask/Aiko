#include "ppu.h"

#include "cartridge.h"
#include "bus.h"

namespace nes
{
    void Ppu::reset()
    {

    }

    Byte Ppu::cpu_read(Word addr, bool rdonly)
    {
        Byte data = 0x00;
        switch (addr)
        {
        case 0x0000: // Control
            break;
        case 0x0001: // Mask
            break;
        case 0x0002: // Status
            break;
        case 0x0003: // OAM Address
            break;
        case 0x0004: // OAM Data
            break;
        case 0x0005: // Scroll
            break;
        case 0x0006: // PPU Address
            break;
        case 0x0007: // PPU Data
        break;
        }
        return data;
    }

    void Ppu::cpu_write(Word addr, Byte data)
    {
        switch (addr)
        {
        case 0x0000: // Control
            break;
        case 0x0001: // Mask
            break;
        case 0x0002: // Status
            break;
        case 0x0003: // OAM Address
            break;
        case 0x0004: // OAM Data
            break;
        case 0x0005: // Scroll
            break;
        case 0x0006: // PPU Address
            break;
        case 0x0007: // PPU Data
            break;
        }
    }

    Byte Ppu::ppu_read(Word addr, bool readonly)
    {
        Byte data = 0x00;
        addr &= 0x3FFF;
        Cartridge* cart = bus->getMicroprocesor<Cartridge>();
        if (cart->ppu_read(addr, data))
        {

        }
        return data;
    }

    void Ppu::ppu_write(Word addr, Byte data)
    {
        addr &= 0x3FFF;
        Cartridge* cart = bus->getMicroprocesor<Cartridge>();
        if (cart->ppu_write(addr, data))
        {

        }
    }
}