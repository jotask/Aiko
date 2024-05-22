#pragma once

#include <mutex>

#include "nes/nes_types.h"
#include "nes/microprocessor.h"
#include "nes/cpu/instructions.h"

namespace nes
{
    namespace test
    {
        class NesTest;
        namespace online
        {
            class OnlinesTest;
        }
    }
    class Memory;

    class Cpu : public Microprocessor
    {
        friend class test::online::OnlinesTest;
        friend class test::NesTest;
    public:

        virtual void reset() override;
        void clock();

        AddressModes currentAddressMode();
        Instruction currentInstruction();

        enum StatusFlags
        {
            C = (1 << 0),   // Carry Bit
            Z = (1 << 1),   // Zero
            I = (1 << 2),   // Disable Interrupts
            D = (1 << 3),   // Decimal Mode (unused in this implementation)
            B = (1 << 4),   // Break
            U = (1 << 5),   // Unused
            V = (1 << 6),   // Overflow
            N = (1 << 7),   // Negative
        };

        Word line = 0;

    private:

        void fetchData();

        void setCurrentAddressMode(AddressModes);
        void setCurrentInstruction(Instruction);

        std::mutex      m_currentAddressMode_mutex;
        std::mutex      m_currentInstruction_mutex;
        AddressModes    m_currentAddressMode;
        Instruction     m_currentInstruction;

        // Addressing modes
        void relative();
        void implied();
        void immediate();
        void zeroPage();
        void zeroPageX();
        void zeroPageY();
        void absolute();
        void absoluteX();
        void absoluteY();
        void indirect();
        void indirectX();
        void indirectY();

        // Instructions
        void adc(); // Add with carry
        void and(); // Logical AND
        void asl(); // Arithmetic Shift Left
        void bcc(); // branch if carry clear
        void bcs(); // Branch if carry set
        void beq(); // Branch if equal
        void bit(); // bit test
        void bmi(); // Branch if Minus
        void bne(); // Branch if not equal
        void bpl(); // Branch if positive
        void brk(); // Force interrupt
        void bvc(); // Branch if overflow clear
        void bvs(); // Branch if overflow set
        void clc(); // Clear carry flag
        void cld(); // Clear decimal mode
        void cli(); // clear interrupt disable
        void clv(); // Clear overflow
        void cmp(); // Compare
        void cpx(); // Compare X register
        void cpy(); // Compare Y register
        void dec(); // Decrement memory
        void dex(); // Decrement X register
        void dey(); // Decrement Y register
        void eor(); // Exclusive OR
        void inc(); // Increment memory
        void inx(); // Increment X register
        void iny(); // Increment Y register
        void jmp(); // Jump
        void jsr(); // Jump to subroutine
        void lda(); // Load accumulator
        void ldx(); // Load X register
        void ldy(); // Load Y register
        void lsr(); // Logical shift right
        void nop(); // No operation
        void ora(); // Logical inclusive OR
        void pha(); // Push accumulator
        void php(); // Push processor status
        void pla(); // Pull accumulator
        void plp(); // Pull processor status
        void rol(); // Rotate left
        void ror(); // Rotate right
        void rti(); // Return from interrupt
        void rts(); // Return from subroutine
        void sbc(); // Substract with carry
        void sec(); // Set carry flag
        void sed(); // Set decimal flag
        void sei(); // Set interrupt disable
        void sta(); // Store accumulator
        void stx(); // Store X register
        void sty(); // Store Y register
        void tax(); // Transfer accumulator to X
        void tay(); // Transfer accumulator to Y
        void txa(); // Transfer X to accumulator
        void tya(); // Transfer Y to accumulator
        void tsx(); // Transfer stack pointer to X
        void txs(); // Transfer X to stack pointer
        void xxx(); // Unofficial OpCode

        void execute(Byte opCode);

        Word addr_abs; // All used memory addresses end up in here
        Word addr_rel;   // Represents absolute address following a branch
        Byte memoryFetched;

        Word program_counter;
        Byte stack_pointer;

        // Registers
        Byte A, X, Y;

        Byte P;

        Byte waitForCycles;

        // helpers

        Byte read(Word);
        void write(Word, Byte);

        Word getWordStackAddress() const;
        void assertStackAddress();
        void pushStack(Byte, bool = false);
        void pushWordStack(Word, bool = false);
        Byte peekStack();
        Byte popStack(bool = true);
        Word popWordStack();
        uint8_t getFlag(StatusFlags p);
        void    setFlag(StatusFlags p, bool v);
        inline constexpr Byte getP() { return P; }
        inline constexpr void setP(Byte p) { P = p; }

    };

}
