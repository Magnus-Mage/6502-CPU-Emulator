#include "cpu6502/cpu.hpp"
#include <print>
#include "cpu6502/opcodes.hpp"

namespace cpu6502
{

[[nodiscard]] auto CPU::execute(i32 cycles, Memory& memory) -> std::expected<i32, EmulatorError>
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
    std::println("DEBUG: About to fetch from PC = 0x{:04X}", pc_);

    auto ins_result = fetch_byte(cycles, memory);
    if (!ins_result)
        return std::unexpected(ins_result.error());

    const auto opcode = static_cast<Opcode>(ins_result.value());
    std::println("DEBUG: Fetched opcode = 0x{:02X}", static_cast<u8>(opcode));
    switch (opcode)
        {
                // Load Accumulator

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
                */

                // Load X Register

            case Opcode::LDX_IM:
                return execute_ldx_immediate(cycles, memory);

            case Opcode::LDX_ZP:
                return execute_ldx_zero_page(cycles, memory);

            case Opcode::LDX_ZPY:
                return execute_ldx_zero_page_y(cycles, memory);

            case Opcode::LDX_ABS:
                return execute_ldx_absolute(cycles, memory);

            case Opcode::LDX_ABSY:
                return execute_ldx_absolute_y(cycles, memory);

                // Load Y Register

            case Opcode::LDY_IM:
                return execute_ldy_immediate(cycles, memory);

            case Opcode::LDY_ZP:
                return execute_ldy_zero_page(cycles, memory);

            case Opcode::LDY_ZPX:
                return execute_ldy_zero_page_x(cycles, memory);

            case Opcode::LDY_ABS:
                return execute_ldy_absolute(cycles, memory);

            case Opcode::LDY_ABSX:
                return execute_ldy_absolute_x(cycles, memory);

                // Add With Carry

            case Opcode::ADC_IM:
                return execute_adc_immediate(cycles, memory);

            case Opcode::ADC_ZP:
                return execute_adc_zero_page(cycles, memory);

            case Opcode::ADC_ZPX:
                return execute_adc_zero_page_x(cycles, memory);

            case Opcode::ADC_ABS:
                return execute_adc_absolute(cycles, memory);

            case Opcode::ADC_ABSX:
                return execute_adc_absolute_x(cycles, memory);

            case Opcode::ADC_ABSY:
                return execute_adc_absolute_y(cycles, memory);

            case Opcode::ADC_INDX:
                return execute_adc_indirect_x(cycles, memory);

            case Opcode::ADC_INDY:
                return execute_adc_indirect_y(cycles, memory);

                // Logical AND

            case Opcode::AND_IM:
                return execute_and_immediate(cycles, memory);

            case Opcode::AND_ZP:
                return execute_and_zero_page(cycles, memory);

            case Opcode::AND_ZPX:
                return execute_and_zero_page_x(cycles, memory);

            case Opcode::AND_ABS:
                return execute_and_absolute(cycles, memory);

            case Opcode::AND_ABSX:
                return execute_and_absolute_x(cycles, memory);

            case Opcode::AND_ABSY:
                return execute_and_absolute_y(cycles, memory);

            case Opcode::AND_INDX:
                return execute_and_indirect_x(cycles, memory);

            case Opcode::AND_INDY:
                return execute_and_indirect_y(cycles, memory);

                // Exclusive OR

            case Opcode::EOR_IM:
                return execute_eor_immediate(cycles, memory);

            case Opcode::EOR_ZP:
                return execute_eor_zero_page(cycles, memory);

            case Opcode::EOR_ZPX:
                return execute_eor_zero_page_x(cycles, memory);

            case Opcode::EOR_ABS:
                return execute_eor_absolute(cycles, memory);

            case Opcode::EOR_ABSX:
                return execute_eor_absolute_x(cycles, memory);

            case Opcode::EOR_ABSY:
                return execute_eor_absolute_y(cycles, memory);

            case Opcode::EOR_INDX:
                return execute_eor_indirect_x(cycles, memory);

            case Opcode::EOR_INDY:
                return execute_eor_indirect_y(cycles, memory);

                // ASL - Arithmetic Shift Left

            case Opcode::ASL_A:
                return execute_shift_left_accumulator(cycles);

            case Opcode::ASL_ZP:
                return execute_shift_left_zero_page(cycles, memory);

            case Opcode::ASL_ZPX:
                return execute_shift_left_zero_page_x(cycles, memory);

            case Opcode::ASL_ABS:
                return execute_shift_left_absolute(cycles, memory);

            case Opcode::ASL_ABSX:
                return execute_shift_left_absolute_x(cycles, memory);

                // Clear Flags

            case Opcode::CLC:
                return clear_carry_flag(cycles);

            case Opcode::CLD:
                return clear_decimal_mode(cycles);

            case Opcode::CLI:
                return clear_interrupt_disable(cycles);

            case Opcode::CLV:
                return clear_overflow_flag(cycles);

            // Branch Instructions
            case Opcode::BCC:
                return execute_bcc(cycles, memory);

            case Opcode::BCS:
                return execute_bcs(cycles, memory);

            case Opcode::BEQ:
                return execute_beq(cycles, memory);

            case Opcode::BIT_ZP:
                return execute_bit_zero_page(cycles, memory);

            case Opcode::BIT_ABS:
                return execute_bit_absolute(cycles, memory);

            case Opcode::BMI:
                return execute_bmi(cycles, memory);

            case Opcode::BNE:
                return execute_bne(cycles, memory);

            case Opcode::BPL:
                return execute_bpl(cycles, memory);

            case Opcode::BRK:
                return execute_brk(cycles, memory);

            case Opcode::BVC:
                return execute_bvc(cycles, memory);

            case Opcode::BVS:
                return execute_bvs(cycles, memory);

                // Comparision Instructions
                // CMP - Compare

            case Opcode::CMP_IM:
                return execute_cmp_immediate(cycles, memory);

            case Opcode::CMP_ZP:
                return execute_cmp_zero_page(cycles, memory);

            case Opcode::CMP_ZPX:
                return execute_cmp_zero_page_x(cycles, memory);

            case Opcode::CMP_ABS:
                return execute_cmp_absolute(cycles, memory);

            case Opcode::CMP_ABSX:
                return execute_cmp_absolute_x(cycles, memory);

            case Opcode::CMP_ABSY:
                return execute_cmp_absolute_y(cycles, memory);

            case Opcode::CMP_INDX:
                return execute_cmp_indirect_x(cycles, memory);

            case Opcode::CMP_INDY:
                return execute_cmp_indirect_y(cycles, memory);

                // CPX - Compare X Register

            case Opcode::CPX_IM:
                return execute_cpx_immediate(cycles, memory);

            case Opcode::CPX_ZP:
                return execute_cpx_zero_page(cycles, memory);

            case Opcode::CPX_ABS:
                return execute_cpx_absolute(cycles, memory);

                // CPY - Compare Y Register

            case Opcode::CPY_IM:
                return execute_cpy_immediate(cycles, memory);

            case Opcode::CPY_ZP:
                return execute_cpy_zero_page(cycles, memory);

            case Opcode::CPY_ABS:
                return execute_cpy_absolute(cycles, memory);

                // Control Flow Instructions

            case Opcode::JSR:
                return execute_jsr(cycles, memory);

            case Opcode::RTS:
                return execute_rts(cycles, memory);
            /*
            case Opcode::JMP_ABS:
                return execute_jmp_absolute(cycles, memory);

            case Opcode::JMP_IND:
                return execute_jmp_indirect(cycles, memory);
            */
            default:
                std::println("Unhandled opcode: 0x{:02X}", static_cast<u8>(opcode));
                return std::unexpected(EmulatorError::InvalidOpcode);
        }
}

}  // namespace cpu6502
