#include "nes/cpu/cpu.h"

#include "nes/bus.h"
#include "nes/memory.h"
#include "nes/utils/nes_utils.h"
#include <aiko_includes.h>

namespace nes
{

    static std::string stack_print_padding = "    ";

    Word Cpu::getWordStackAddress() const
    {
        const Word stack_address = 0x0100 + stack_pointer;
        return stack_address;
    }

    void Cpu::assertStackAddress()
    {
        const Word stack_address = getWordStackAddress();
        assert(stack_address <= std::get<1>(Memory::STACK_PAGE), "Stack Overflow!");
        assert(stack_address >= std::get<0>(Memory::STACK_PAGE), "Stack Underflow!");
    }

    void Cpu::pushStack(Byte value, bool late)
    {
        if (late == false) stack_pointer--;
        const Word stack_address = getWordStackAddress();
        if (late == true) stack_pointer--;
        aiko::Log::trace(stack_print_padding, "pushStack: ", unsigned(stack_address), " Value: ", unsigned(value) );
        assertStackAddress();
        write(stack_address, value);
    }

    void Cpu::pushWordStack(Word value, bool late)
    {
        const Byte high = getHigh(value);
        const Byte low  = getLow(value);
        aiko::Log::trace(stack_print_padding, "pushWordStack() Value: ", toString(value));
        stack_print_padding = "        ";
        pushStack(high, late);
        pushStack(low, late);
        stack_print_padding = "    ";
    }

    Byte Cpu::peekStack()
    {
        const Word stack_address = getWordStackAddress();
        assertStackAddress();
        return read(stack_address);
    }

    Byte Cpu::popStack(bool late)
    {
        if (late == false) stack_pointer++;
        const Word stack_address = getWordStackAddress();
        if (late == true) stack_pointer++;
        assertStackAddress();
        const Byte result = read(stack_address);
        aiko::Log::trace(stack_print_padding, "popStack: ", toString(stack_address), " Value: ", toString(result));
        return result;
    }

    Word Cpu::popWordStack()
    {
        aiko::Log::trace(stack_print_padding, "popWordStack() ");
        stack_print_padding = "        ";
        const Byte low = popStack(false);
        const Byte high = popStack(false);
        stack_print_padding = "    ";
        const Word result = toWord(high, low);
        aiko::Log::trace(stack_print_padding, "popWordStack() ", toString(result));
        return result;
    }

}
