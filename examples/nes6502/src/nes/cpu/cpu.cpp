#include "nes/cpu/cpu.h"

#include "nes/bus.h"
#include "nes/memory.h"

#include <aiko_includes.h>
#include "nes/utils/nes_utils.h"
#include "nes/tests/nes_test_log.h"

namespace nes
{

    void Cpu::reset()
    {

        m_cycles = 0;

#if false
        // Get address to set program counter to
        addr_abs = 0xC000;
        Word lo = getMemory()->read(Word(addr_abs + 0));
        Word hi = getMemory()->read(Word(addr_abs + 1));
        // Set it
        program_counter = toWord(hi, lo);
#else
        // TODO Upon reset the address at 0xFFFC is loaded into the program counter, and that is where the emulation begins
        program_counter = 0xC000;
#endif

        // Reset internal registers
        A = 0;
        X = 0;
        Y = 0;
        stack_pointer = 0xFD;
        P = 0x24; // FIXME 0x24 due nest test check, needs investigation
        setFlag(U, true);

        // Clear internal helper variables
        addr_rel = 0x0000;
        addr_abs = 0x0000;
        memoryFetched = 0x00;

        // Reset takes time
        waitForCycles = 8;

    }

    void Cpu::clock()
    {
        m_cycles++;
        if (waitForCycles > 0)
        {
            waitForCycles--;
            return;
        }
        Byte opCode = read(program_counter++);
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
            memoryFetched = read(addr_abs);
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

    Byte Cpu::read(Word address)
    {
        return bus->cpu_read(address, false);
    }

    void Cpu::write(Word address, Byte data)
    {
        bus->cpu_write(address, data);
    }

    uint8_t Cpu::getFlag(StatusFlags p) const
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
