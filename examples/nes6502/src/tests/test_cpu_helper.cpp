#include "test_cpu.h"

#include <assert.h>

#include "bus.h"
#include "cpu.h"
#include "memory.h"
#include "cartridge.h"
#include "test_type.h"
#include "nes_utils.h"

namespace test
{

    void CPUTest::set_memory_address_zero_page(nes::nes6502* n, nes::Byte address, nes::Byte value)
    {
        nes::Bus* bus = n->getBus();
        nes::Memory* mem = bus->getMicroprocesor<nes::Memory>();
        mem->write(address, value);
    }

    void CPUTest::set_memory_address_zero_page_x(nes::nes6502* n, nes::Byte address, nes::Byte value)
    {
        nes::Bus* bus = n->getBus();
        nes::Cpu* cpu = bus->getMicroprocesor<nes::Cpu>();
        nes::Memory* mem = bus->getMicroprocesor<nes::Memory>();
        nes::Word finalAddress = (address + cpu->X);
        mem->write( finalAddress, value);
    }

    void CPUTest::set_memory_address_absolute(nes::nes6502* n, nes::Word address, nes::Byte value)
    {
        nes::Bus* bus = n->getBus();
        nes::Cpu* cpu = bus->getMicroprocesor<nes::Cpu>();
        nes::Memory* mem = bus->getMicroprocesor<nes::Memory>();
        mem->write(address, value);
    }

    void CPUTest::set_memory_address_absolute_x(nes::nes6502* n, nes::Word address, nes::Byte value)
    {
        nes::Bus* bus = n->getBus();
        nes::Cpu* cpu = bus->getMicroprocesor<nes::Cpu>();
        nes::Memory* mem = bus->getMicroprocesor<nes::Memory>();
        mem->write(nes::Word(address + cpu->X), value);
    }

    void CPUTest::set_memory_address_absolute_y(nes::nes6502* n, nes::Word address, nes::Byte value)
    {
        nes::Bus* bus = n->getBus();
        nes::Cpu* cpu = bus->getMicroprocesor<nes::Cpu>();
        nes::Memory* mem = bus->getMicroprocesor<nes::Memory>();
        mem->write(nes::Word(address + cpu->Y), value);
    }

    void CPUTest::set_memory_address_indirect_x(nes::nes6502* n, nes::Word address, nes::Byte value)
    {
        nes::Bus* bus = n->getBus();
        nes::Cpu* cpu = bus->getMicroprocesor<nes::Cpu>();
        nes::Memory* mem = bus->getMicroprocesor<nes::Memory>();
        nes::Byte high = nes::getHigh(value);
        nes::Byte low = nes::getLow(value);
        nes::Word effectiveAddress = nes::Word(address + cpu->X);
        mem->write(effectiveAddress, high);
        effectiveAddress++; // TODO ensure correct wrap-around for zero page addressing (?)
        mem->write(effectiveAddress, low);
        nes::Word targetAddress = nes::toWord(high, low);
        mem->write(targetAddress, value);
    }

    void CPUTest::set_memory_address_indirect_y(nes::nes6502* n, nes::Word address, nes::Byte value)
    {
        nes::Bus* bus = n->getBus();
        nes::Cpu* cpu = bus->getMicroprocesor<nes::Cpu>();
        nes::Memory* mem = bus->getMicroprocesor<nes::Memory>();
        nes::Byte high = nes::getHigh(value);
        nes::Byte low = nes::getLow(value);
        nes::Word effectiveAddress = nes::Word(address + cpu->Y);
        mem->write(effectiveAddress, high);
        effectiveAddress++; // TODO ensure correct wrap-around for zero page addressing (?)
        mem->write(effectiveAddress, low);
        nes::Word targetAddress = nes::toWord(high, low);
        mem->write(targetAddress, value);
    }

}
