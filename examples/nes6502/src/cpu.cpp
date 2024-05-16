#include "cpu.h"

#include "bus.h"
#include "memory.h"

#include <aiko_includes.h>
#include "nes_utils.h"

namespace nes
{

    void Cpu::reset()
    {

#if false
        // Get address to set program counter to
        addr_abs = 0xC000;
        Word lo = getMemory()->read(Word(addr_abs + 0));
        Word hi = getMemory()->read(Word(addr_abs + 1));
        // Set it
        program_counter = toWord(hi, lo);
#else
        program_counter = 0xC000;
#endif

        // Reset internal registers
        A = 0;
        X = 0;
        Y = 0;
        stack_pointer = 0xFD;
        P = 0x00 | U;

        // Clear internal helper variables
        addr_rel = 0x0000;
        addr_abs = 0x0000;
        memoryFetched = 0x00;

        // Reset takes time
        waitForCycles = 8;

    }

    void Cpu::clock()
    {

        if (waitForCycles > 0)
        {
            waitForCycles--;
            return;
        }

        Memory* memory = bus->getMicroprocesor<Memory>();

        Byte opCode = memory->read( program_counter++ );

        setFlag(U, true);
        execute(opCode);
        setFlag(U, true);


    }

    AddressModes Cpu::currentAddressMode()
    {
        m_currentAddressMode_mutex.lock();
        auto tmp = m_currentAddressMode;
        m_currentAddressMode_mutex.unlock();
        return tmp;
    }

    Instruction Cpu::currentInstruction()
    {
        m_currentInstruction_mutex.lock();
        auto tmp =  m_currentInstruction;
        m_currentInstruction_mutex.unlock();
        return tmp;
    }

    void Cpu::fetchData()
    {
        if (currentAddressMode() != AddressModes::Implied)
        {
            memoryFetched = getMemory()->read(addr_abs);
        }
    }

    void Cpu::setCurrentAddressMode(AddressModes address )
    {
        m_currentAddressMode_mutex.lock();
        m_currentAddressMode = address;
        m_currentAddressMode_mutex.unlock();
    }

    void Cpu::setCurrentInstruction(Instruction instruction)
    {
        m_currentInstruction_mutex.lock();
        m_currentInstruction = instruction;
        m_currentInstruction_mutex.unlock();
    }

    Memory* Cpu::getMemory()
    {
        Memory* mem = bus->getMicroprocesor<Memory>();
        assert(mem != nullptr, "Memory not found in buffer");
        return mem;
    }

    uint8_t Cpu::getFlag(StatusFlags p)
    {
        return ((P & p) > 0) ? 1 : 0;
    }

    void Cpu::setFlag(StatusFlags p, bool v)
    {
        if (v)
        {
            P |= p;
        }
        else
        {
            P &= ~p;
        }
    }

}
