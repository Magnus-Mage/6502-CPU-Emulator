#pragma once

#include "types.hpp"

namespace cpu6502
{

/**
 * @type enum class
 * @brief opcodes (type-safe enum)
 */
enum class Opcode : u8
{
    // LDA - Load Accumlator
    LDA_IM   = 0xA9,  // Load Accumulator - Immediate
    LDA_ZP   = 0xA5,  // Load Accumulator - Zero Page
    LDA_ZPX  = 0xB5,  // Load Accumulator - Zero Page, X
    LDA_ABS  = 0xAD,  // Load Accumulator - ABSOLUTE
    LDA_ABSX = 0xBD,  // Load Accumulator - ABSOLUTE, X
    LDA_ABSY = 0xB9,  // Load Accumulator - ABSOLUTE, Y
    LDA_INDX = 0xA1,  // Load Accumulator - INDIRECT, X
    LDA_INDY = 0xB1,  // Load Accumulator - INDIRECT, Y

    // LDX - Load X Register
    LDX_IM   = 0xA2,  // Load X Register - Immediate
    LDX_ZP   = 0xA6,  // Load X Register - Zero Page
    LDX_ZPY  = 0xB6,  // Load X Register - Zero Page, Y
    LDX_ABS  = 0xAE,  // Load X Register - Absolute
    LDX_ABSY = 0xBE,  // Load X Register - Absolute, Y

    // LDY - Load Y Register
    LDY_IM   = 0xA0,  // Load Y Register - Immediate
    LDY_ZP   = 0xA4,  // Load Y Register - Zero Page
    LDY_ZPX  = 0xB4,  // Load Y Register - Zero Page, X
    LDY_ABS  = 0xAC,  // Load Y Register - Absolute
    LDY_ABSX = 0xBC,  // Load Y Register - Absolute, X

    // ADC - Add With Carry
    ADC_IM   = 0x69,  // Add With Carry - Immediate
    ADC_ZP   = 0x65,  // Add With Carry - Zero Page
    ADC_ZPX  = 0x75,  // Add With Carry - Zero Page, X
    ADC_ABS  = 0x6D,  // Add With Carry - Absolute
    ADC_ABSX = 0x7D,  // Add With Carry - Absolute, X
    ADC_ABSY = 0x79,  // Add With Carry - Absolute, Y
    ADC_INDX = 0x61,  // Add With Carry - Indirect, X
    ADC_INDY = 0x71,  // Add With Carry - Indirect, Y

    // AND - Logical AND
    AND_IM   = 0x29,  // Logical AND - Immediate
    AND_ZP   = 0x25,  // Logical AND - Zero Page
    AND_ZPX  = 0x35,  // Logical AND - Zero Page, X
    AND_ABS  = 0x2D,  // Logical AND - Absolute
    AND_ABSX = 0x3D,  // Logical AND - Absolute, X
    AND_ABSY = 0x39,  // Logical AND - Absolute, Y
    AND_INDX = 0x21,  // Logical AND - Indirect, X
    AND_INDY = 0x31,  // Logical AND - Indirect, Y

    // ASL - Arithmetic Shift Left
    ASL_A    = 0x0A,  // Arithmetic Shift Left - Accumulator
    ASL_ZP   = 0x06,  // Arithmetic Shift Left - Zero Page
    ASL_ZPX  = 0x16,  // Arithmetic Shift Left - Zero Page, X
    ASL_ABS  = 0x0E,  // Arithmetic Shift Left - Absolute
    ASL_ABSX = 0x1E,  // Arithmetic Shift Left - Absolute, X

    // Clear Flags
    CLC = 0x18,  // CLC - Clear Carry Flag - Implied
    CLD = 0xD8,  // CLD - Clear Decimal Mode - Implied
    CLI = 0x58,  // CLI - Clear Interrupt Disable
    CLV = 0xB8,  // CLV - Clear Overflow Flag

    // Control Flow
    JSR     = 0x20,  // Jump to Subroutine
    RTS     = 0x60,  // Return from Subroutine
    JMP_ABS = 0x4c,  // Jump to address with abolute addressing
    JMP_IND = 0x6c,  // Jump to address with indirect addressing
};

/**
 * @brief Helper to get opcode name
 */
constexpr const char* opcode_name(Opcode op) noexcept
{
    switch (op)
        {
            // LDA
            case Opcode::LDA_IM:
                return "LDA_IM";
            case Opcode::LDA_ZP:
                return "LDA_ZP";
            case Opcode::LDA_ZPX:
                return "LDA_ZPX";
            case Opcode::LDA_ABS:
                return "LDA_ABS";
            case Opcode::LDA_ABSX:
                return "LDA_ABSX";
            case Opcode::LDA_ABSY:
                return "LDA_ABSY";
            // LDX
            case Opcode::LDX_IM:
                return "LDX_IM";
            case Opcode::LDX_ZP:
                return "LDX_ZP";
            case Opcode::LDX_ZPY:
                return "LDX_ZPY";
            case Opcode::LDX_ABS:
                return "LDX_ABS";
            case Opcode::LDX_ABSY:
                return "LDX_ABSY";
            // LDY
            case Opcode::LDY_IM:
                return "LDY_IM";
            case Opcode::LDY_ZP:
                return "LDY_ZP";
            case Opcode::LDY_ZPX:
                return "LDY_ZPX";
            case Opcode::LDY_ABS:
                return "LDY_ABS";
            case Opcode::LDY_ABSX:
                return "LDY_ABSX";
            // ADC
            case Opcode::ADC_IM:
                return "ADC_IM";
            case Opcode::ADC_ZP:
                return "ADC_ZP";
            case Opcode::ADC_ZPX:
                return "ADC_ZPX";
            case Opcode::ADC_ABS:
                return "ADC_ABS";
            case Opcode::ADC_ABSX:
                return "ADC_ABSX";
            case Opcode::ADC_ABSY:
                return "ADC_ABSY";
            case Opcode::ADC_INDX:
                return "ADC_INDX";
            case Opcode::ADC_INDY:
                return "ADC_INDY";
            // AND
            case Opcode::AND_IM:
                return "AND_IM";
            case Opcode::AND_ZP:
                return "AND_ZP";
            case Opcode::AND_ZPX:
                return "AND_ZPX";
            case Opcode::AND_ABS:
                return "AND_ABS";
            case Opcode::AND_ABSX:
                return "AND_ABSX";
            case Opcode::AND_ABSY:
                return "AND_ABSY";
            case Opcode::AND_INDX:
                return "AND_INDX";
            case Opcode::AND_INDY:
                return "AND_INDY";

                // Arthmetic Shift Left
            case Opcode::ASL_A:
                return "ASL_A";
            case Opcode::ASL_ZP:
                return "ASL_ZP";
            case Opcode::ASL_ZPX:
                return "ASL_ZPX";
            case Opcode::ASL_ABS:
                return "ASL_ABS";
            case Opcode::ASL_ABSX:
                return "ASL_ABSX";

            // Clear Flags
            case Opcode::CLC:
                return "CLC";
            case Opcode::CLD:
                return "CLD";
            case Opcode::CLI:
                return "CLI";
            case Opcode::CLV:
                return "CLV";

            // Control FLow
            case Opcode::JSR:
                return "JSR";
            case Opcode::RTS:
                return "RTS";

            default:
                return "UNKNOWN";
        }
}

}  // namespace cpu6502
