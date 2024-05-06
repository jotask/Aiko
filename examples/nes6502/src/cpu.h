#pragma once

#include "nes_types.h"
#include "microprocessor.h"
#include "instructions.h"

namespace test { class CPUTest; }
namespace test { namespace online { class OnlinesTest; } }

namespace nes
{
    class Memory;
    class Cpu : public Microprocessor
    {
        friend class test::CPUTest;
        friend class test::online::OnlinesTest;
    public:

        virtual void reset() override;
        virtual void clock() override;

        AddressModes currentAddressMode() const;
        Instruction currentInstruction() const;

    private:

        AddressModes m_currentAddressMode;
        Instruction m_currentInstruction;

        void pushStack(Byte);
        Byte popStack();

        // Addressing modes
        void relative();
        void implicit();
        void implied();
        void accumulator();
        void inmediate();
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
        void brk(); // Force interrup
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
        void tya(); // Tranfer Y to accumulator
        void tsx(); // Transfer stack pointer to X
        void txs(); // Transfer X to stack pointer

        void execute(Byte opCode);

        Byte memoryFetched;

        Word program_counter;
        Word stack_pointer;

        // Registers
        Byte A, X, Y;

        // StatusFlags
        Byte C : 1; // Carry Status
        Byte Z : 1; // Zero Status
        Byte I : 1; // Interrup Status
        Byte D : 1; // Decimal Status
        Byte B : 1; // Break Status
        Byte V : 1; // Overflow Status
        Byte N : 1; // Negative Status

        Byte waitForCycles;

        // helpers
        Memory* getMemory();
        inline constexpr Byte getP() { return (C << 7) | (Z << 6) | (I << 5) | (D << 4) | (B << 3) | (V << 1) | (N << 0); }
        inline constexpr void setP(Byte p)
        {
            C = (p >> 0) & 0x01; // Carry flag (bit 0)
            Z = (p >> 1) & 0x01; // Zero flag (bit 1)
            I = (p >> 2) & 0x01; // Interrupt disable flag (bit 2)
            D = (p >> 3) & 0x01; // Decimal mode flag (bit 3)
            B = (p >> 4) & 0x01; // Break command flag (bit 4)
            // Bit 5 is unused and is typically set to 1
            V = (p >> 6) & 0x01; // Overflow flag (bit 6)
            N = (p >> 7) & 0x01; // Negative flag (bit 7)
        }

    };

}
