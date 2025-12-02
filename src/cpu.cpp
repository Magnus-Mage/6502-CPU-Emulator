#include "cpu6502/cpu.hpp"
#include "cpu6502/opcodes.hpp"
#include <print>

namespace cpu6502
{
constexpr auto CPU::reset(Memory& memory) noexcept
{
	pc_ = RESET_VECTOR;
	sp_ = INITIAL_SP;
	a_  = 0;
	x_  = 0;
	y_  = 0;
	flags_ = StatusFlags{};
	memory.clear();
}

[[nodiscard]] auto CPU::execute(i32 cycles, Memory& memory)
	-> std::expected<i32, EmulatorError>
{
	const i32 cycles_requested = cycles;

	while (cycles > 0)
	{
		auto result = fetch_and_execute(cycles, memory);
		if (!result) 
		{
			return std::unexpected(result.error());
		}
	}

	return cycles_requested - cycles;
}

constexpr auto CPU::fetch_byte(i32& cycles, Memory& memory)
	->std::expected<u8, EmulatorError>
{
	auto result = memory.read_byte(pc_);
	if (!result) return result;
	pc_++;
	cycles--;

	return result;
}

constexpr auto CPU::fetch_word(i32& cycles, Memory& memory)
	-> std::expected<u16, EmulatorError>
{
	auto result = memory.read_word(pc_);
	if (!result) return result;
	pc_ += 2;
	cycles -= 2;

	return result;
}

constexpr auto CPU::read_byte(i32& cycles, u16 address, Memory& memory)
	-> std::expected<u8, EmulatorError>
{
	cycles--;
	return memory.read_word(address);
}

constexpr auto CPU::push_byte(i32& cycles, u8 value, Memory& memory)
	-> std::expected<void, EmulatorError>
{
	if (sp_ == 0) return std::unexpected(EmulatorError::StackUnderflow);

	auto result = memory.write_byte(INITIAL_SP + sp_, value);
	if (!result) return result;
	sp_--;
	cycles--;
	return {};
}

constexpr auto CPU::pop_byte(i32& cycles, Memory& memory)
	-> std::expected<u8, EmulatorError>
{
	if (sp_ == 0xFF) return std::unexpected(EmulatorError::StackOverflow);

	sp_++;
	cycles--;
	return memory.read_byte(STACK_PAGE + sp_);
}

constexpr void CPU::set_zn_flags(u8 value) noexcept
{
	flags_.zero 	= (value == 0);
	flags_.negative 	= (value & 0b10000000) != 0;
}

constexpr void CPU::load_accumulator (u8 value) noexcept
{
	a_ = value;
	set_zn_flags(a_);
}

constexpr auto CPU::fetch_and_execute(i32& cycles, Memory& memory)
	-> std::expected<void, EmulatorError>
{
	auto ins_result = fetch_byte(cycles, memory);
	if (!ins_result) return std::unexpected(ins_result.error());

	const auto opcode = static_cast<Opcode>(ins_result.value());

	switch(opcode)
	{
		case Opcode::LDA_IM:
			return execute_lda_immediate(cycles, memory);
		
		case Opcode::LDA_ZP:
			return execute_lda_zero_page(cycles, memory);

		case Opcode::LDA_ZPX:
			return execute_lda_zero_page_x(cycles, memory);

		case Opcode::LDA_ABS:
			return execute_lda_absolute(cycles, memory);

		case Opcode::LDA_ABSX:
			return execute_lda_absolute_x(cycles, memory);

		case Opcode::LDA_ABSY:
			return execute_lda_absolute_y(cycles, memory);
		/*
		case Opcode::INDX:
			return execute_lda_indirect_x(cycles, memory);

		case Opcode::INDY:
			return execute_lda_indirect_y(cycles, memory);

		case Opcode::JSR:
			return execute_jsr(cycles, memory);

		case Opcode::RTS:
			return execute_rts(cycles, memory);

		case Opcode::JMP_ABS:
			return execute_jmp_absolute(cycles, memory);

		case Opcode::JMP_IND:
			return execute_jmp_indirect(cycles, memory);
		*/
		default:
			std::println("Unhandled opcode: 0x{:02X}",
					static_cast<u8>(opcode));
			return std::unexpected(EmulatorError::InvalidOpcode);
	}
}



} // namespace cpu6502
