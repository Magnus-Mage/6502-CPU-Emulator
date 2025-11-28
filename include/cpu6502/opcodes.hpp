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
        LDA_IM          =       0xA9;           // Load Accumulator - Immediate
        LDA_ZP          =       0xA5;           // Load Accumulator - Zero Page
        LDA_ZPX         =       0xB5;           // Load Accumulator - Zero Page, X
        LDA_ABS         =       0xAD;           // Load Accumulator - ABSOLUTE
        LDA_ABSX        =       0xBD;           // Load Accumulator - ABSOLUTE, X
        LDA_ABSY        =       0xB9;           // Load Accumulator - ABSOLUTE, Y
        LDA_INDX        =       0xA1;           // Load Accumulator - INDIRECT, X
        LDA_INDY        =       0xB1;           // Load Accumulator - INDIRECT, Y
	
	// LDX - Load X Register
	// LDY - Load Y Register
	

	// Control Flow 
        JSR             =       0x20;           // Jump to Subroutine
        RTS             =       0x60;           // Return from Subroutine
	JMP_ABS		= 	0x4c;		// Jump to address with abolute addressing
	JMP_IND		=	0x6c;		// Jump to address with indirect addressing
};

/**
 * @brief Helper to get opcode name
 */
constexpr const char* opcode_name (Opcode op) noexpect
{
	switch (op)
	{
		case Opcode::LDA_IM:   return "LDA_IM";
        	case Opcode::LDA_ZP:   return "LDA_ZP";
        	case Opcode::LDA_ZPX:  return "LDA_ZPX";
        	case Opcode::LDA_ABS:  return "LDA_ABS";
        	case Opcode::JSR:      return "JSR";
        	case Opcode::RTS:      return "RTS";
        	default:               return "UNKNOWN";
	}
}

} // namespace cpu6502
