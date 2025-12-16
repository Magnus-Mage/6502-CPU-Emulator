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
        LDA_IM          =       0xA9,           // Load Accumulator - Immediate
        LDA_ZP          =       0xA5,           // Load Accumulator - Zero Page
        LDA_ZPX         =       0xB5,           // Load Accumulator - Zero Page, X
        LDA_ABS         =       0xAD,           // Load Accumulator - ABSOLUTE
        LDA_ABSX        =       0xBD,           // Load Accumulator - ABSOLUTE, X
        LDA_ABSY        =       0xB9,           // Load Accumulator - ABSOLUTE, Y
        LDA_INDX        =       0xA1,           // Load Accumulator - INDIRECT, X
        LDA_INDY        =       0xB1,           // Load Accumulator - INDIRECT, Y
	
	// LDX - Load X Register
	LDX_IM		=	0xA2,		// Load X Register - Immediate 
	LDX_ZP		=	0xA6,		// Load X Register - Zero Page
	LDX_ZPY		=	0xB6,		// Load X Register - Zero Page, Y
	LDX_ABS		=	0xAE,		// Load X Register - Absolute
	LDX_ABSY	=	0xBE,		// Load X Register - Absolute, Y
	
	// LDY - Load Y Register
	LDY_IM		=	0xA0,		// Load Y Register - Immediate 
	LDY_ZP		=	0xA4,		// Load Y Register - Zero Page
	LDY_ZPX		=	0xB4,		// Load Y Register - Zero Page, X
	LDY_ABS		=	0xAC,		// Load Y Register - Absolute
	LDY_ABSX	=	0xBC,		// Load Y Register - Absolute, X
		

	// Control Flow 
        JSR             =       0x20,           // Jump to Subroutine
        RTS             =       0x60,           // Return from Subroutine
	JMP_ABS		= 	0x4c,		// Jump to address with abolute addressing
	JMP_IND		=	0x6c,		// Jump to address with indirect addressing
};

/**
 * @brief Helper to get opcode name
 */
constexpr const char* opcode_name (Opcode op) noexcept
{
	switch (op)
	{
		// LDA
		case Opcode::LDA_IM:   return "LDA_IM";
        	case Opcode::LDA_ZP:   return "LDA_ZP";
        	case Opcode::LDA_ZPX:  return "LDA_ZPX";
        	case Opcode::LDA_ABS:  return "LDA_ABS";
		case Opcode::LDA_ABSX: return "LDA_ABSX";
		case Opcode::LDA_ABSY: return "LDA_ABSY";
		// LDX
		case Opcode::LDX_IM:   return "LDX_IM";
		case Opcode::LDX_ZP:   return "LDX_ZP";
		case Opcode::LDX_ZPY:  return "LDX_ZPY";
		case Opcode::LDX_ABS:  return "LDX_ABS";
		case Opcode::LDX_ABSY: return "LDX_ABSY";
		// LDY
		case Opcode::LDY_IM:   return "LDY_IM";
		case Opcode::LDY_ZP:   return "LDY_ZP";
		case Opcode::LDY_ZPX:  return "LDY_ZPX";
		case Opcode::LDY_ABS:  return "LDY_ABS";
		case Opcode::LDY_ABSX: return "LDY_ABSX";
		
        	case Opcode::JSR:      return "JSR";
        	case Opcode::RTS:      return "RTS";
		
        	default:               return "UNKNOWN";
	}
}

} // namespace cpu6502
