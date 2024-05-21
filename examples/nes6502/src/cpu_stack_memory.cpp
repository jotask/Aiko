#include "cpu.h"

#include "bus.h"
#include "memory.h"
#include "nes_utils.h"
#include <aiko_includes.h>

namespace nes
{

    static std::string stack_print_padding = "    ";

    Word Cpu::getWordStackAddress()
    {
        Word stack_address = 0x0100 + stack_pointer;
        return stack_address;
    }

    void Cpu::assertStackAddress()
    {
        Word stack_address = getWordStackAddress();
        assert(stack_address <= std::get<1>(Memory::STACK_PAGE), "Stack Overflow!");
        assert(stack_address >= std::get<0>(Memory::STACK_PAGE), "Stack Underflow!");
    }

    void Cpu::pushStack(Byte value, bool late)
    {
        if (late == false) stack_pointer--;
        Word stack_address = getWordStackAddress();
        if (late == true) stack_pointer--;
        aiko::Log::trace(stack_print_padding, "pushStack: ", toString(stack_address), " Value: ", toString(value) );
        assertStackAddress();
        getMemory()->write(stack_address, value);
    }

    void Cpu::pushWordStack(Word value, bool late)
    {
        const Byte high = getHigh(value);
        const Byte low  = getLow(value);
        aiko::Log::trace(stack_print_padding, "pushWordStack() Value: ", toString(value));
        stack_print_padding = "        ";
        pushStack(low, late);
        pushStack(high, late);
        stack_print_padding = "    ";
    }

    Byte Cpu::peekStack()
    {
        Word stack_address = getWordStackAddress();
        assertStackAddress();
        return getMemory()->read(stack_address);
    }

    Byte Cpu::popStack(bool late)
    {
        if (late == false) stack_pointer++;
        Word stack_address = getWordStackAddress();
        if (late == true) stack_pointer++;
        assertStackAddress();
        auto* memory = getMemory();
        Byte result = getMemory()->read(stack_address);
        aiko::Log::trace(stack_print_padding, "popStack: ", toString(stack_address), " Value: ", toString(result));
        return result;
    }

    Word Cpu::popWordStack()
    {
        aiko::Log::trace(stack_print_padding, "popWordStack() ");
        stack_print_padding = "        ";
        Word stack_address = getWordStackAddress();
        Byte high = popStack();
        Byte low = popStack();
        stack_print_padding = "    ";
        Word result = toWord(high, low);
        aiko::Log::trace(stack_print_padding, "popWordStack() ", toString(result));
        return result;
    }

}
