#include "cpu.h"

#include "bus.h"
#include "memory.h"
#include "nes_utils.h"
#include <aiko_includes.h>

namespace nes
{
    void Cpu::pushStack(Byte value)
    {
        aiko::Log::trace("Push to stack ", toString(value));
        stack_pointer--;
        assert(stack_pointer <= std::get<1>(Memory::STACK_PAGE), "TODO Stack Overflow!");
        getMemory()->write(Word(stack_pointer), value);
    }

    Byte Cpu::popStack()
    {
        aiko::Log::trace("Pop to stack");
        stack_pointer++;
        assert(stack_pointer >=std::get<0>(Memory::STACK_PAGE), "Stack Underflow!");
        return getMemory()->read( Word(0x01 + stack_pointer));
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
        Memory* mem = getMemory();
        memoryFetched = mem->read(program_counter++);
    }

    void Cpu::zeroPage()
    {
        // The 16-bit address space available to the 6502 is thought to consist of 256 "pages" of
        // 256 memory locations each ($00…$FF). In this model the high-byte of an address gives the
        // page number and the low-byte a location inside this page.
        m_currentAddressMode = AddressModes::ZeroPage;
        Memory* mem = getMemory();
        Byte memoryAddress = mem->read(program_counter++);
        memoryFetched = mem->read(memoryAddress);
    }

    void Cpu::zeroPageX()
    {
        // Indexed addressing adds the contents of either the X-register or the Y-register to the
        // provided address to give the effective address, which provides the operand.
        m_currentAddressMode = AddressModes::ZeroPageX;
        Memory* mem = getMemory();
        Byte memoryAddress = mem->read(program_counter++);
        Word tmp = memoryAddress + X;
        memoryFetched = mem->read(tmp);
    }

    void Cpu::zeroPageY()
    {
        // Indexed addressing adds the contents of either the X-register or the Y-register to the
        // provided address to give the effective address, which provides the operand.
        m_currentAddressMode = AddressModes::ZeroPageY;
        Memory* mem = getMemory();
        Byte memoryAddress = mem->read(program_counter++);
        memoryFetched = mem->read(memoryAddress) + Y;
    }

    void Cpu::absolute()
    {
        // Absolute addressing modes provides the 16-bit address of a memory location, the contents
        // of which used as the operand to the instruction.
        m_currentAddressMode = AddressModes::Absolute;
        Memory* mem = getMemory();
        Byte high = mem->read(program_counter++);
        Byte low  = mem->read(program_counter++);
        Word absoluteMemory = toWord(high, low);
        memoryFetched = mem->read(absoluteMemory);
    }

    void Cpu::absoluteX()
    {
        m_currentAddressMode = AddressModes::AbsoluteX;
        Memory* mem = getMemory();
        Byte high = mem->read(program_counter++);
        Byte low = mem->read(program_counter++);
        Word absoluteMemory = toWord(high, low) + X;
        memoryFetched = mem->read(absoluteMemory);
    }

    void Cpu::absoluteY()
    {
        m_currentAddressMode = AddressModes::AbsoluteY;
        Memory* mem = getMemory();
        Byte high = mem->read(program_counter++);
        Byte low = mem->read(program_counter++);
        Word absoluteMemory = toWord(high, low) + Y;
        memoryFetched = mem->read(absoluteMemory);
    }

    void Cpu::indirect()
    {
        m_currentAddressMode = AddressModes::Indirect;
    }

    void Cpu::indirectX()
    {
        m_currentAddressMode = AddressModes::IndirectX;
        Memory* mem = getMemory();
        // Fetch the zero page address from memory and increment the program counter
        Byte zeropageAddress = mem->read(program_counter++);
        // Calculate the effective address by adding the X register to the zero page address
        Word effectiveAddress = zeropageAddress + X;
        // Read the low of the target address from the effective address
        Byte msb = mem->read(effectiveAddress);

        // Increment the effective address to read the MSB of the target address
        effectiveAddress++;

        // Ensure correct wrap-around for zero page addressing
        if ((effectiveAddress & 0xFF) == 0) {
            effectiveAddress = (effectiveAddress & 0xFF00) | (zeropageAddress & 0xFF);
        }

        // Read the MSB of the target address from the adjusted effective address
        Byte lsb = mem->read(effectiveAddress);

        // Combine the LSB and MSB to form the target address
        Word targetAddress = nes::toWord(msb, lsb);;

        // Read the byte from memory at the target address
        memoryFetched = mem->read(targetAddress);

    }

    void Cpu::indirectY()
    {
        m_currentAddressMode = AddressModes::IndirectY;
        Memory* mem = getMemory();
        // Fetch the zero page address from memory and increment the program counter
        Byte zeropageAddress = mem->read(program_counter++);
        // Calculate the effective address by adding the X register to the zero page address
        Word effectiveAddress = zeropageAddress + Y;
        // Read the low of the target address from the effective address
        Byte msb = mem->read(effectiveAddress);

        // Increment the effective address to read the MSB of the target address
        effectiveAddress++;

        // Ensure correct wrap-around for zero page addressing
        if ((effectiveAddress & 0xFF) == 0) {
            effectiveAddress = (effectiveAddress & 0xFF00) | (zeropageAddress & 0xFF);
        }

        // Read the MSB of the target address from the adjusted effective address
        Byte lsb = mem->read(effectiveAddress);

        // Combine the LSB and MSB to form the target address
        Word targetAddress = nes::toWord(msb, lsb);;

        // Read the byte from memory at the target address
        memoryFetched = mem->read(targetAddress);
    }

}
