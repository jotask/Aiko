#include "cpu.h"

#include "bus.h"
#include "memory.h"
#include "nes_utils.h"

namespace nes
{

    inline Memory* getMemory(Bus* bus)
    {
        Memory* mem = bus->getMicroprocesor<Memory>();
        assert( mem != nullptr , "Memory not found in buffer");
        return mem;
    }

    void Cpu::relative()
    {
        m_currentAddressMode = AddressModes::Relative;
    }

    void Cpu::implicit()
    {
        // m_currentAddressMode = AddressModes::Implicit;
    }

    void Cpu::implied()
    {
        // These instructions act directly on one or more registers or flags internal to the CPU.
        // Therefor, these instructions are principally single-byte instructions, lacking an
        // explicit operand. The operand is implied, as it is already provided by the very instruction.
        m_currentAddressMode = AddressModes::Implied;
    }

    void Cpu::accumulator()
    {
        // m_currentAddressMode = AddressModes::;
    }

    void Cpu::inmediate()
    {
        // A literal operand is given immediately after the instruction.The operand is always
        // an 8 - bit value and the total instruction length is always 2 bytes.
        m_currentAddressMode = AddressModes::Inmediate;
        Memory* mem = getMemory(bus);
        memoryFetched = mem->read(program_counter++);
    }

    void Cpu::zeroPage()
    {
        m_currentAddressMode = AddressModes::ZeroPage;
    }

    void Cpu::zeroPageX()
    {
        m_currentAddressMode = AddressModes::ZeroPageX;
    }

    void Cpu::zeroPageY()
    {
        m_currentAddressMode = AddressModes::ZeroPageY;
    }

    void Cpu::absolute()
    {
        // Absolute addressing modes provides the 16-bit address of a memory location, the contents
        // of which used as the operand to the instruction.
        m_currentAddressMode = AddressModes::Absolute;
        Memory* mem = getMemory(bus);
        Byte high = mem->read(program_counter++);
        Byte low  = mem->read(program_counter++);
        Word absoluteMemory = toWord(high, low);
        memoryFetched = mem->read(absoluteMemory);
    }

    void Cpu::absoluteX()
    {
        m_currentAddressMode = AddressModes::AbsoluteX;
    }

    void Cpu::absoluteY()
    {
        m_currentAddressMode = AddressModes::AbsoluteY;
    }

    void Cpu::indirect()
    {
        m_currentAddressMode = AddressModes::Indirect;
    }

    void Cpu::indirectX()
    {
        m_currentAddressMode = AddressModes::IndirectX;
    }

    void Cpu::indirectY()
    {
        m_currentAddressMode = AddressModes::IndirectY;
    }

}
