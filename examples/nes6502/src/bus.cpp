#include "bus.h"

#include "cartridge.h"
#include "ppu.h"

namespace nes
{
    Bus::Bus()
    {
    }

    void Bus::reset()
    {
        for (auto& m : microprocessors)
        {
            m->reset();
        }
    }

    void Bus::addMicroprocesor(Microprocessor* m)
    {
        m->bus = this;
        microprocessors.push_back(m);
    }

    void Bus::clock()
    {
        for(auto& m : microprocessors )
        {
            m->clock();
        }
    }
    void Bus::cpu_write(Word address, Byte data)
    {
        if (getMicroprocesor<Cartridge>()->cpu_write(address, data))
        {
            // The cartridge "sees all" and has the facility to veto
            // the propagation of the bus transaction if it requires.
            // This allows the cartridge to map any address to some
            // other data, including the facility to divert transactions
            // with other physical devices. The NES does not do this
            // but I figured it might be quite a flexible way of adding
            // "custom" hardware to the NES in the future!
        }
        else if (address >= 0x0000 && address <= 0x1FFF)
        {
            // System RAM Address Range. The range covers 8KB, though
            // there is only 2KB available. That 2KB is "mirrored"
            // through this address range. Using bitwise AND to mask
            // the bottom 11 bits is the same as addr % 2048.
            cpu_ram[address & 0x07FF] = data;

        }
        else if (address >= 0x2000 && address <= 0x3FFF)
        {
            // PPU Address range. The PPU only has 8 primary registers
            // and these are repeated throughout this range. We can
            // use bitwise AND operation to mask the bottom 3 bits, 
            // which is the equivalent of addr % 8.
            getMicroprocesor<Ppu>()->cpu_write(address & 0x0007, data);
        }
    }

    Byte Bus::cpu_read(Word address, bool read_only)
    {
        Byte data;
        if (getMicroprocesor<Cartridge>()->cpu_read(address, data))
        {
            // Cartridge data
        }
        else if (address >= 0x0000 && address <= 0x1FFF)
        {
            // System RAM Address Range, mirrored every 2048
            data = cpu_ram[address & 0x07FF];
        }
        else if (address >= 0x2000 && address <= 0x3FFF)
        {
            // PPU Address range, mirrored every 8
            data = getMicroprocesor<Ppu>()->cpu_read(address & 0x0007, read_only);
        }
        return data;
    }
}
