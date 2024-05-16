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
        Word stack_address = 0x0100 + stack_pointer;
        assert(stack_address <= std::get<1>(Memory::STACK_PAGE), "TODO Stack Overflow!");
        getMemory()->write(stack_address, value);
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
        setCurrentAddressMode(AddressModes::Relative);
        addr_rel = getMemory()->read(program_counter++);
        if (addr_rel & 0x80)
        {
            addr_rel |= 0xFF00;
        }
    }

    void Cpu::implied()
    {
        // These instructions act directly on one or more registers or flags internal to the CPU.
        // Therefor, these instructions are principally single-byte instructions, lacking an
        // explicit operand. The operand is implied, as it is already provided by the very instruction.
        setCurrentAddressMode(AddressModes::Implied);
        memoryFetched = A;
    }

    void Cpu::inmediate()
    {
        // A literal operand is given immediately after the instruction.The operand is always
        // an 8 - bit value and the total instruction length is always 2 bytes.
        setCurrentAddressMode(AddressModes::Inmediate);
        addr_abs = program_counter++;
    }

    void Cpu::zeroPage()
    {
        // The 16-bit address space available to the 6502 is thought to consist of 256 "pages" of
        // 256 memory locations each ($00…$FF). In this model the high-byte of an address gives the
        // page number and the low-byte a location inside this page.
        setCurrentAddressMode(AddressModes::ZeroPage);
        addr_abs = getMemory()->read(program_counter++);
        addr_abs = getLow(addr_abs);
    }

    void Cpu::zeroPageX()
    {
        // Indexed addressing adds the contents of either the X-register or the Y-register to the
        // provided address to give the effective address, which provides the operand.
        setCurrentAddressMode(AddressModes::ZeroPageX);
        addr_abs = (getMemory()->read(program_counter) + X);
        program_counter++;
        addr_abs &= 0x00FF;
    }

    void Cpu::zeroPageY()
    {
        // Indexed addressing adds the contents of either the X-register or the Y-register to the
        // provided address to give the effective address, which provides the operand.
        setCurrentAddressMode(AddressModes::ZeroPageY);
        addr_abs = (getMemory()->read(program_counter) + Y);
        program_counter++;
        addr_abs &= 0x00FF;
    }

    void Cpu::absolute()
    {
        // Absolute addressing modes provides the 16-bit address of a memory location, the contents
        // of which used as the operand to the instruction.
        setCurrentAddressMode(AddressModes::Absolute);
        Byte lo = getMemory()->read(program_counter++);
        Byte hi = getMemory()->read(program_counter++);
        addr_abs = toWord(hi, lo);
    }

    void Cpu::absoluteX()
    {
        setCurrentAddressMode(AddressModes::AbsoluteX);
        Word lo = getMemory()->read(program_counter);
        program_counter++;
        Word hi = getMemory()->read(program_counter);
        program_counter++;

        addr_abs = (hi << 8) | lo;
        addr_abs += X;

        if ((addr_abs & 0xFF00) != (hi << 8))
        {
            aiko::Log::error("Not Implemented");
            waitForCycles++;
        }
    }

    void Cpu::absoluteY()
    {
        setCurrentAddressMode(AddressModes::AbsoluteY);
        uint16_t lo = getMemory()->read(program_counter);
        program_counter++;
        uint16_t hi = getMemory()->read(program_counter);
        program_counter++;

        addr_abs = (hi << 8) | lo;
        addr_abs += Y;

        if ((addr_abs & 0xFF00) != (hi << 8))
        {
            waitForCycles++;
        }

    }

    void Cpu::indirect()
    {
        setCurrentAddressMode(AddressModes::Indirect);
        Word ptr_lo = getMemory()->read(program_counter);
        program_counter++;
        Word ptr_hi = getMemory()->read(program_counter);
        program_counter++;

        Word ptr = (ptr_hi << 8) | ptr_lo;

        if (ptr_lo == 0x00FF) // Simulate page boundary hardware bug
        {
            addr_abs = (getMemory()->read(Word(ptr & 0xFF00)) << 8) | getMemory()->read(Word(ptr + 0));
        }
        else // Behave normally
        {
            addr_abs = (getMemory()->read(Word(ptr + 1)) << 8) | getMemory()->read(Word(ptr + 0));
        }

    }

    void Cpu::indirectX()
    {
        setCurrentAddressMode(AddressModes::IndirectX);
        Word t = getMemory()->read(program_counter);
        program_counter++;

        Word lo = getMemory()->read(Word((Word)(t + (Word)X) & 0x00FF));
        Word hi = getMemory()->read(Word((Word)(t + (Word)X + 1) & 0x00FF));

        addr_abs = (hi << 8) | lo;

    }

    void Cpu::indirectY()
    {
        setCurrentAddressMode(AddressModes::IndirectY);
        uint16_t t = getMemory()->read(program_counter);
        program_counter++;

        uint16_t lo = getMemory()->read(Word(t & 0x00FF));
        uint16_t hi = getMemory()->read(Word((t + 1) & 0x00FF));

        addr_abs = (hi << 8) | lo;
        addr_abs += Y;

        if ((addr_abs & 0xFF00) != (hi << 8))
        {
            waitForCycles++;
        }
    }

}
