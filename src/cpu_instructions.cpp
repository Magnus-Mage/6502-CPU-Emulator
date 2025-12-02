#include "cpu6502/cpu.hpp"

namespace cpu6502
{

constexpr auto CPU::execute_lda_immediate(i32& cycles, Memory& memory)
       -> std::expected<void, EmulatorError>
{
	auto value = fetch_byte(cycles, memory);

	if (!value) return std::unexpected(value.error());

	load_accumulator(value.value());
	return {} ;
}       

constexpr auto CPU::execute_lda_zero_page(i32& cycles, Memory& memory)
	-> std::expected<void, EmulatorError>
{
	auto address = fetch_byte(cycles, memory);
	if (!address) return std::unexpected(address.error());

	auto value = read_byte(cycles, address.value(), memory);
	if (!value) return std::unexpected(value.error());

	load_accumulator(value.value());
	return {} ;
}

constexpr auto CPU::execute_lda_zero_page_x(i32& cycles, Memory& memory)
	-> std::expected<void, EmulatorError>
{
	auto address = fetch_byte(cycles, memory);
	if (!address) return std::unexpected(address.error());

	u8 final_address = address.value() + x_;
	cycles--;

	auto value = read_byte(cycles, final_address, memory);
	if (!value) return std::unexpected(value.error());

	load_accumulator(value.value());
	return {} ;
}

constexpr auto CPU::execute_lda_absolute(i32& cycles, Memory& memory)
	-> std::expected<void, EmulatorError>
{
	auto address = fetch_word(cycles, memory);
	if (!address) return std::unexpected(address.error());

	auto value = read_byte(cycles, address.value(), memory);
	if (!value) return std::unexpected(value.error());
	
	load_accumulator(value.value());

	return {} ;	
}


constexpr auto CPU::execute_lda_absolute_x(i32& cycles, Memory& memory)
	-> std::expected<void, EmulatorError>
{
	auto address = fetch_word(cycles, memory);
	if (!address) return std::unexpected(address.error());

	u16 final_address = address.value() + x_;

	auto value = read_byte(cycles, final_address, memory);
	if (!value) return std::unexpected(value.error());

	if (page_crossed(address.value(), final_address))
	{
		cycles--;
	}

	load_accumulator(value.value());
	return {} ;
}

constexpr auto CPU::execute_lda_absolute_y(i32& cycles, Memory& memory)
	-> std::expected<void, EmulatorError>
{
	auto address = fetch_word(cycles, memory);
	if (!address) return std::unexpected(address.error());

	u16 final_address = address.value() + y_;

	auto value = read_byte(cycles, final_address, memory);
	if (!value) return std::unexpected(value.error());

	if (page_crossed(address.value(), final_address))
	{
		cycles--;
	}

	load_accumulator(value.value());
	return {} ;
}

constexpr auto CPU::execute_jsr(i32& cycles, Memory& memory)
	-> std::expected<void, EmulatorError>
{
	auto sub_address = fetch_word(cycles, memory);
	if (!sub_address) return std::unexpected(sub_address.error());

	const u16 return_address = pc_ - 1;

	cycles--; // Cycle 4: internal cycle
	
	auto push_high = push_byte(cycles, static_cast<u8>(return_address >> 8), memory);
	if (!push_high) return push_high;

	auto push_low = push_byte(cycles, static_cast<u8>(return_address & 0xFF), memory);
	if (!push_low) return push_low;

	pc_ = sub_address.value();

	return {} ;
}

constexpr auto CPU::execute_rts(i32& cycles, Memory& memory)
	-> std::expected<void, EmulatorError>
{
	cycles--; // Cycle 2: Internal cycle
	
	auto low = pop_byte(cycles, memory);
	if (!low) return std::unexpected(low.error());

	auto high = pop_byte(cycles, memory);
	if (!high) return std::unexpected(high.error());

	const u16 return_address = low.value() | (static_cast<u16>(high.value()) << 8);

	pc_ = return_address + 1;

	cycles -= 2;		// Cycles 5-6: Increment PC and internal operations

	return {} ;
}

} // namespace cpu6502
