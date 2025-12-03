#include "cpu6502/cpu.hpp"
#include "cpu6502/opcodes.hpp"
#include <print>

namespace cpu6502
{

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
