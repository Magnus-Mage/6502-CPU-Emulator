#include <cstdint>
#include <array>
#include <optional>
#include <expected>
#include <format>
#include <print>
#include <stdexpect>
#include <concepts>


/**
 * Modern type alias using standard fixed-width types
 */
using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

/**
 * @type Enum class
 * @brief Enum class for better error handling 
 */
enum class EmulatorError
{
	InvalidAddress,
	StackOverflow,
	InvalidOpcode,
	StackUnderflow,
	InsufficientCycles
};

/**
 * @brief concept for the address types
 */
template<template T>
concept AddressType = std::unsigned_integral<T> && sizeof(T) <= sizeof(u16);

/**
 * @type class
 * @brief Memory class that will define our memory subsystem
 */
class Memory
{
public:
	static constexpr u32 MAX_MEM = 1024 * 64;

	constexpr Memory() : data_{} {} 	// Zero Initialise

	// Read byte
	[[nodiscard]] constexpr auto read_byte (u16 address) const
		-> std::expected<u8, EmulatorError>	
	{
		if (address >= MAX_MEM)
		{
			return std::unexpected(EmulatorError::InvalidAddress);
		}

		return data_[address];
	}
	
	// Write byte
	constexpr auto write_byte (u16 address, u8 value)
		-> std::expected<void, EmulatorError>
	{
		if (address >= MAX_MEM)
		{
			return std::unexpected(EmulatorError::InvalidAddress);
		}

		data_[address] = value;

		return {};
	}

	// Write word (little endian)
	constexpr auto write_word (u16 address, u16 value)
		-> std::expected<void, EmulatorError>
	{
		if (address + 1 >= MAX_MEM)
		{
			return std::unexpected(EmulatorError::InvalidAddress);
		}

		data_[address]     = static_cast<u8>(value & 0xFF);
		data_[address + 1] = static_cast<u8>(value >> 8);
		return {};
	}

	// Read word (little endian)
	[[nodiscard]] constexpr auto read_word (u16 address)
		-> std::expected<u16, EmulatorError>
	{
		if (address + 1 >= MAX_MEM)
		{
			return std::unexpected(EmulatorError::InvalidAddress)
		}

		u16 low  = data_[address];
		u16 high = data_[address + 1];

		return low | (high << 8);
	}

	// Clear all memory
	constexpr void clear() noexcept
	{
		data_.fill(0);
	}

	// Direct access for setup 
	constexpr operator[](u16 address) noexcept
	{
		return data_[address];
	}

private:
	std::array<u8, MAX_MEM> data_;
};


/**
 * @type struct
 * @brief status flags with field names
 */
struct StatusFlags
{
	bool carry     : 1 = false;
	bool zero      : 1 = false;
	bool interrupt : 1 = false;
	bool decimal   : 1 = false;
	bool brk       : 1 = false;
	bool overflow  : 1 = false;
	bool negative  : 1 = false;

	constexpr StatusFlags() = default;

	// Convert to byte representation
	[[nodiscard]] constexpr u8 to_byte() const noexcept
	{
		return (carry 	  << 0) |
		       (zero      << 1) |
		       (interrupt << 1) |
	      	       (decimal   << 1) |
		       (brk       << 1) |
		       (overflow  << 1) |
		       (negative  << 1);
	}
};

/**
 * @type enum class
 * @brief opcodes (type-safe enum)
 */
enum class Opcode : u8
{
	LDA_IM		= 	0xA9;		// Load Accumulator - Immediate
	LDA_ZP		= 	0xA5;		// Load Accumulator - Zero Page
	LDA_ZPX		= 	0xB5;		// Load Accumulator - Zero Page, X
	LDA_ABS		= 	0xAD;		// Load Accumulator - ABSOLUTE
	LDA_ABSX	= 	0xBD;		// Load Accumulator - ABSOLUTE, X
	LDA_ABSY	= 	0xB9;		// Load Accumulator - ABSOLUTE, Y
	LDA_INDX	= 	0xA1;		// Load Accumulator - INDIRECT, X
	LDA_INDY	= 	0xB1;		// Load Accumulator - INDIRECT, Y
	JSR		= 	0x20;		// Jump to Subroutine
	RTS 		= 	0x60;		// Return from Subroutine
};

/**
 * @type class
 * @brief CPU class --> Handles all the cpu core functionality
 */
class CPU
{
public:


}
