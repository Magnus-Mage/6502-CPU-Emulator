#include <gtest/gtest.h>
#include "cpu6502/cpu.hpp"
#include "cpu6502/memory.hpp"
#include "cpu6502/opcodes.hpp"

using namespace cpu6502;

class ADCTest : public ::testing::Test {
protected:
    Memory mem;
    CPU cpu;

    void SetUp() override {
        mem[0xFFFC] = 0x00;
        mem[0xFFFD] = 0x80;
        cpu.reset(mem);
    }
};

// ============================================================================
// Basic Addition Tests
// ============================================================================

TEST_F(ADCTest, ADC_Immediate_SimpleAddition) {
    // given:
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x05;
    mem[0x8002] = static_cast<u8>(Opcode::ADC_IM);
    mem[0x8003] = 0x03;
    
    // when:
    auto result = cpu.execute(4, mem);
    
    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0x08);
    EXPECT_FALSE(cpu.get_flags().carry);
    EXPECT_FALSE(cpu.get_flags().zero);
    EXPECT_FALSE(cpu.get_flags().negative);
    EXPECT_FALSE(cpu.get_flags().overflow);
}

TEST_F(ADCTest, ADC_Immediate_WithCarryFlagSet) {
    // given:
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x05;
    mem[0x8002] = static_cast<u8>(Opcode::ADC_IM);
    mem[0x8003] = 0x03;
    
    (void)cpu.execute(2, mem);  // Load 0x05
    cpu.set_flag_c(true);  // Set carry
    
    // when:
    auto result = cpu.execute(2, mem);  // Execute ADC
    
    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0x09);  // 5 + 3 + 1 = 9
}

// ============================================================================
// Carry Flag Tests
// ============================================================================

TEST_F(ADCTest, ADC_SetsCarryOnOverflow) {
    // given: 255 + 1 = 256 (overflow)
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0xFF;
    mem[0x8002] = static_cast<u8>(Opcode::ADC_IM);
    mem[0x8003] = 0x01;
    
    // when:
    auto result = cpu.execute(4, mem);
    
    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0x00);  // Wraps to 0
    EXPECT_TRUE(cpu.get_flags().carry);
    EXPECT_TRUE(cpu.get_flags().zero);
    EXPECT_FALSE(cpu.get_flags().negative);
}

TEST_F(ADCTest, ADC_CarryChainAddition) {
    // given: Simulate multi-byte addition
    // First byte: 255 + 1 = 256 (carry out)
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0xFF;
    mem[0x8002] = static_cast<u8>(Opcode::ADC_IM);
    mem[0x8003] = 0x01;
    
    // Second byte: 0 + 0 + carry
    mem[0x8004] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8005] = 0x00;
    mem[0x8006] = static_cast<u8>(Opcode::ADC_IM);
    mem[0x8007] = 0x00;
    
    // when:
    (void) cpu.execute(4, mem);  // First addition
    EXPECT_TRUE(cpu.get_flags().carry);
    
    auto result = cpu.execute(4, mem);  // Second addition with carry
    
    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0x01);  // 0 + 0 + 1(carry) = 1
}

// ============================================================================
// Zero Flag Tests
// ============================================================================

TEST_F(ADCTest, ADC_SetsZeroFlag) {
    // given: 0 + 0 = 0
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x00;
    mem[0x8002] = static_cast<u8>(Opcode::ADC_IM);
    mem[0x8003] = 0x00;
    
    // when:
    auto result = cpu.execute(4, mem);
    
    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0x00);
    EXPECT_TRUE(cpu.get_flags().zero);
}

// ============================================================================
// Negative Flag Tests
// ============================================================================

TEST_F(ADCTest, ADC_SetsNegativeFlag) {
    // given: Result has bit 7 set
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x80;  // -128 in signed
    mem[0x8002] = static_cast<u8>(Opcode::ADC_IM);
    mem[0x8003] = 0x01;
    
    // when:
    auto result = cpu.execute(4, mem);
    
    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0x81);
    EXPECT_TRUE(cpu.get_flags().negative);
}

// ============================================================================
// Overflow Flag Tests (Signed Arithmetic)
// ============================================================================

TEST_F(ADCTest, ADC_SetsOverflowFlag_PositivePlusPositiveEqualsNegative) {
    // given: 80 + 80 = 160, but in signed: 80 + 80 = -96 (overflow!)
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x50;  // +80
    mem[0x8002] = static_cast<u8>(Opcode::ADC_IM);
    mem[0x8003] = 0x50;  // +80
    
    // when:
    auto result = cpu.execute(4, mem);
    
    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0xA0);  // 160 = -96 in signed
    EXPECT_TRUE(cpu.get_flags().overflow);  // Signed overflow occurred
    EXPECT_TRUE(cpu.get_flags().negative);
    EXPECT_FALSE(cpu.get_flags().carry);
}

TEST_F(ADCTest, ADC_SetsOverflowFlag_NegativePlusNegativeEqualsPositive) {
    // given: -80 + -80 = -160, but wraps to +96 (overflow!)
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0xB0;  // -80 in two's complement
    mem[0x8002] = static_cast<u8>(Opcode::ADC_IM);
    mem[0x8003] = 0xB0;  // -80
    
    // when:
    auto result = cpu.execute(4, mem);
    
    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0x60);  // +96 in signed
    EXPECT_TRUE(cpu.get_flags().overflow);
    EXPECT_FALSE(cpu.get_flags().negative);
    EXPECT_TRUE(cpu.get_flags().carry);
}

TEST_F(ADCTest, ADC_NoOverflow_PositivePlusNegative) {
    // given: Overflow only when same signs produce different sign
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x50;  // +80
    mem[0x8002] = static_cast<u8>(Opcode::ADC_IM);
    mem[0x8003] = 0xB0;  // -80
    
    // when:
    auto result = cpu.execute(4, mem);
    
    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(cpu.get_flags().overflow);  // No overflow for different signs
}

// ============================================================================
// Addressing Mode Tests
// ============================================================================

TEST_F(ADCTest, ADC_ZeroPage) {
    // given:
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x10;
    mem[0x8002] = static_cast<u8>(Opcode::ADC_ZP);
    mem[0x8003] = 0x42;  // Zero page address
    mem[0x0042] = 0x05;  // Value at address
    
    // when:
    auto result = cpu.execute(5, mem);  // 2 + 3 cycles
    
    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0x15);  // 0x10 + 0x05
}

TEST_F(ADCTest, ADC_ZeroPageX) {
    // given:
    cpu.set_x(0x05);
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x20;
    mem[0x8002] = static_cast<u8>(Opcode::ADC_ZPX);
    mem[0x8003] = 0x10;  // Base address
    mem[0x0015] = 0x08;  // Value at 0x10 + 0x05
    
    // when:
    auto result = cpu.execute(6, mem);  // 2 + 4 cycles
    
    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0x28);  // 0x20 + 0x08
}

TEST_F(ADCTest, ADC_Absolute) {
    // given:
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x15;
    mem[0x8002] = static_cast<u8>(Opcode::ADC_ABS);
    mem[0x8003] = 0x00;  // Low byte
    mem[0x8004] = 0x20;  // High byte ($2000)
    mem[0x2000] = 0x0A;
    
    // when:
    auto result = cpu.execute(6, mem);  // 2 + 4 cycles
    
    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0x1F);  // 0x15 + 0x0A
}

TEST_F(ADCTest, ADC_AbsoluteX_NoPageCross) {
    // given:
    cpu.set_x(0x01);
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x10;
    mem[0x8002] = static_cast<u8>(Opcode::ADC_ABSX);
    mem[0x8003] = 0x00;
    mem[0x8004] = 0x20;  // $2000
    mem[0x2001] = 0x05;  // $2000 + $01
    
    // when:
    auto result = cpu.execute(6, mem);  // 2 + 4 cycles (no page cross)
    
    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0x15);
    EXPECT_EQ(result.value(), 6);
}

TEST_F(ADCTest, ADC_AbsoluteX_WithPageCross) {
    // given:
    cpu.set_x(0xFF);
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x10;
    mem[0x8002] = static_cast<u8>(Opcode::ADC_ABSX);
    mem[0x8003] = 0x02;
    mem[0x8004] = 0x20;  // $2002
    mem[0x2101] = 0x07;  // $2002 + $FF = $2101 (page crossed)
    
    // when:
    auto result = cpu.execute(7, mem);  // 2 + 5 cycles (page cross)
    
    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0x17);
    EXPECT_EQ(result.value(), 7);
}

TEST_F(ADCTest, ADC_AbsoluteY_WithPageCross) {
    // given:
    cpu.set_y(0xFF);
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x10;
    mem[0x8002] = static_cast<u8>(Opcode::ADC_ABSY);
    mem[0x8003] = 0x02;
    mem[0x8004] = 0x20;
    mem[0x2101] = 0x03;
    
    // when:
    auto result = cpu.execute(7, mem);
    
    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0x13);
}

// ============================================================================
// Cycle Count Tests
// ============================================================================

TEST_F(ADCTest, ADC_Immediate_CorrectCycles) {
    mem[0x8000] = static_cast<u8>(Opcode::ADC_IM);
    mem[0x8001] = 0x01;
    
    auto result = cpu.execute(2, mem);
    
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 2);
}

TEST_F(ADCTest, ADC_ZeroPage_CorrectCycles) {
    mem[0x8000] = static_cast<u8>(Opcode::ADC_ZP);
    mem[0x8001] = 0x10;
    mem[0x0010] = 0x05;
    
    auto result = cpu.execute(3, mem);
    
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 3);
}

TEST_F(ADCTest, ADC_Absolute_CorrectCycles) {
    mem[0x8000] = static_cast<u8>(Opcode::ADC_ABS);
    mem[0x8001] = 0x00;
    mem[0x8002] = 0x20;
    mem[0x2000] = 0x01;
    
    auto result = cpu.execute(4, mem);
    
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 4);
}

// ============================================================================
// Real-World Scenarios
// ============================================================================

TEST_F(ADCTest, ADC_MultiByteAddition_16Bit) {
    // Simulate adding two 16-bit numbers: $01FF + $0002 = $0201
    
    // Add low bytes: $FF + $02
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0xFF;
    mem[0x8002] = static_cast<u8>(Opcode::ADC_IM);
    mem[0x8003] = 0x02;
    
    (void)cpu.execute(4, mem);
    EXPECT_EQ(cpu.get_a(), 0x01);  // Low byte result
    EXPECT_TRUE(cpu.get_flags().carry);  // Carry for high byte
    
    // Add high bytes with carry: $01 + $00 + carry
    mem[0x8004] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8005] = 0x01;
    mem[0x8006] = static_cast<u8>(Opcode::ADC_IM);
    mem[0x8007] = 0x00;
    
    auto result = cpu.execute(4, mem);
    
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0x02);  // High byte result
    EXPECT_FALSE(cpu.get_flags().carry);
    
    // Final result: $0201 ✓
}
