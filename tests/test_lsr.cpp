#include <gtest/gtest.h>
#include "cpu6502/cpu.hpp"
#include "cpu6502/memory.hpp"
#include "cpu6502/opcodes.hpp"

using namespace cpu6502;

class LSRTest : public ::testing::Test
{
 protected:
    Memory mem;
    CPU    cpu;

    void SetUp() override
    {
        mem[0xFFFC] = 0x00;
        mem[0xFFFD] = 0x80;
        cpu.reset(mem);
    }
};

// ============================================================================
// LSR Accumulator Tests
// ============================================================================

TEST_F(LSRTest, LSR_Accumulator_ShiftsRight)
{
    // given: 0b00001010 (10) >> 1 = 0b00000101 (5)
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0b00001010;
    mem[0x8002] = static_cast<u8>(Opcode::LSR_A);

    // when:
    auto result = cpu.execute(4, mem);  // 2 cycles LDA + 2 cycles LSR

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0b00000101);
    EXPECT_FALSE(cpu.get_flags().carry);
    EXPECT_FALSE(cpu.get_flags().zero);
    EXPECT_FALSE(cpu.get_flags().negative);  // Always 0 after LSR
}

TEST_F(LSRTest, LSR_Accumulator_SetsCarryFlag)
{
    // given: 0b00000011 (bit 0 set)
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0b00000011;
    mem[0x8002] = static_cast<u8>(Opcode::LSR_A);

    // when:
    auto result = cpu.execute(4, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0b00000001);
    EXPECT_TRUE(cpu.get_flags().carry);  // Bit 0 shifted into carry
    EXPECT_FALSE(cpu.get_flags().zero);
    EXPECT_FALSE(cpu.get_flags().negative);
}

TEST_F(LSRTest, LSR_Accumulator_SetsZeroFlag)
{
    // given: 0b00000001 >> 1 = 0
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0b00000001;
    mem[0x8002] = static_cast<u8>(Opcode::LSR_A);

    // when:
    auto result = cpu.execute(4, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0);
    EXPECT_TRUE(cpu.get_flags().carry);
    EXPECT_TRUE(cpu.get_flags().zero);
    EXPECT_FALSE(cpu.get_flags().negative);
}

TEST_F(LSRTest, LSR_Accumulator_NegativeFlagAlwaysClear)
{
    // given: Even with bit 7 set initially, LSR clears it
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0b11111111;
    mem[0x8002] = static_cast<u8>(Opcode::LSR_A);

    // when:
    auto result = cpu.execute(4, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0b01111111);
    EXPECT_TRUE(cpu.get_flags().carry);
    EXPECT_FALSE(cpu.get_flags().zero);
    EXPECT_FALSE(cpu.get_flags().negative);  // Bit 7 is now 0
}

TEST_F(LSRTest, LSR_Accumulator_NoCarryWhenBit0Clear)
{
    // given: 0b00001010 (bit 0 clear)
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0b00001010;
    mem[0x8002] = static_cast<u8>(Opcode::LSR_A);

    // when:
    auto result = cpu.execute(4, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0b00000101);
    EXPECT_FALSE(cpu.get_flags().carry);
}

// ============================================================================
// LSR Zero Page Tests
// ============================================================================

TEST_F(LSRTest, LSR_ZeroPage_ShiftsMemory)
{
    // given:
    mem[0x8000] = static_cast<u8>(Opcode::LSR_ZP);
    mem[0x8001] = 0x42;
    mem[0x0042] = 0b00001100;

    // when:
    auto result = cpu.execute(5, mem);  // 5 cycles for LSR ZP

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(mem[0x0042], 0b00000110);
    EXPECT_FALSE(cpu.get_flags().carry);
    EXPECT_FALSE(cpu.get_flags().zero);
    EXPECT_FALSE(cpu.get_flags().negative);
}

TEST_F(LSRTest, LSR_ZeroPage_UpdatesFlags)
{
    // given:
    mem[0x8000] = static_cast<u8>(Opcode::LSR_ZP);
    mem[0x8001] = 0x10;
    mem[0x0010] = 0b11000001;

    // when:
    auto result = cpu.execute(5, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(mem[0x0010], 0b01100000);
    EXPECT_TRUE(cpu.get_flags().carry);   // Bit 0 was set
    EXPECT_FALSE(cpu.get_flags().zero);
    EXPECT_FALSE(cpu.get_flags().negative);
}

// ============================================================================
// LSR Zero Page,X Tests
// ============================================================================

TEST_F(LSRTest, LSR_ZeroPageX_ShiftsMemoryWithIndex)
{
    // given:
    cpu.set_x(0x05);
    mem[0x8000] = static_cast<u8>(Opcode::LSR_ZPX);
    mem[0x8001] = 0x10;
    mem[0x0015] = 0b00111100;  // 0x10 + 0x05

    // when:
    auto result = cpu.execute(6, mem);  // 6 cycles for LSR ZP,X

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(mem[0x0015], 0b00011110);
    EXPECT_FALSE(cpu.get_flags().carry);
}

TEST_F(LSRTest, LSR_ZeroPageX_WrapsAround)
{
    // given: Zero page wrapping
    cpu.set_x(0xFF);
    mem[0x8000] = static_cast<u8>(Opcode::LSR_ZPX);
    mem[0x8001] = 0x10;  // 0x10 + 0xFF = 0x0F (wraps)
    mem[0x000F] = 0b00000010;

    // when:
    auto result = cpu.execute(6, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(mem[0x000F], 0b00000001);
}

// ============================================================================
// LSR Absolute Tests
// ============================================================================

TEST_F(LSRTest, LSR_Absolute_ShiftsMemory)
{
    // given:
    mem[0x8000] = static_cast<u8>(Opcode::LSR_ABS);
    mem[0x8001] = 0x00;
    mem[0x8002] = 0x20;  // $2000
    mem[0x2000] = 0b01010101;

    // when:
    auto result = cpu.execute(6, mem);  // 6 cycles for LSR Absolute

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(mem[0x2000], 0b00101010);
    EXPECT_TRUE(cpu.get_flags().carry);
}

// ============================================================================
// LSR Absolute,X Tests
// ============================================================================

TEST_F(LSRTest, LSR_AbsoluteX_ShiftsMemoryWithIndex)
{
    // given:
    cpu.set_x(0x10);
    mem[0x8000] = static_cast<u8>(Opcode::LSR_ABSX);
    mem[0x8001] = 0x00;
    mem[0x8002] = 0x20;  // $2000
    mem[0x2010] = 0b11110000;  // $2000 + $10

    // when:
    auto result = cpu.execute(7, mem);  // 7 cycles for LSR Absolute,X

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(mem[0x2010], 0b01111000);
    EXPECT_FALSE(cpu.get_flags().carry);
}

TEST_F(LSRTest, LSR_AbsoluteX_WritesToCorrectAddress)
{
    // given:
    cpu.set_x(0x05);
    mem[0x8000] = static_cast<u8>(Opcode::LSR_ABSX);
    mem[0x8001] = 0xFF;
    mem[0x8002] = 0x20;  // $20FF
    mem[0x2104] = 0b10101010;  // $20FF + $05 = $2104

    // when:
    auto result = cpu.execute(7, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(mem[0x2104], 0b01010101);
    EXPECT_FALSE(cpu.get_flags().carry);
    EXPECT_FALSE(cpu.get_flags().negative);
}

// ============================================================================
// Edge Case Tests
// ============================================================================

TEST_F(LSRTest, LSR_MultipleShifts)
{
    // given: Shift multiple times to divide by 8
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0b10000000;  // 128
    mem[0x8002] = static_cast<u8>(Opcode::LSR_A);
    mem[0x8003] = static_cast<u8>(Opcode::LSR_A);
    mem[0x8004] = static_cast<u8>(Opcode::LSR_A);  // 128 >> 3 = 16

    // when:
    auto result = cpu.execute(8, mem);  // 2 + 2 + 2 + 2

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0b00010000);  // 16
}

TEST_F(LSRTest, LSR_ZeroValue)
{
    // given: Shifting 0
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x00;
    mem[0x8002] = static_cast<u8>(Opcode::LSR_A);

    // when:
    auto result = cpu.execute(4, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0x00);
    EXPECT_FALSE(cpu.get_flags().carry);
    EXPECT_TRUE(cpu.get_flags().zero);
    EXPECT_FALSE(cpu.get_flags().negative);
}

TEST_F(LSRTest, LSR_AllOnesValue)
{
    // given: 0xFF >> 1
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0xFF;
    mem[0x8002] = static_cast<u8>(Opcode::LSR_A);

    // when:
    auto result = cpu.execute(4, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0x7F);
    EXPECT_TRUE(cpu.get_flags().carry);
    EXPECT_FALSE(cpu.get_flags().zero);
    EXPECT_FALSE(cpu.get_flags().negative);
}

// ============================================================================
// Cycle Count Tests
// ============================================================================

TEST_F(LSRTest, LSR_Accumulator_CorrectCycles)
{
    // given:
    mem[0x8000] = static_cast<u8>(Opcode::LSR_A);

    // when:
    auto result = cpu.execute(2, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 2);
}

TEST_F(LSRTest, LSR_ZeroPage_CorrectCycles)
{
    // given:
    mem[0x8000] = static_cast<u8>(Opcode::LSR_ZP);
    mem[0x8001] = 0x10;
    mem[0x0010] = 0x01;

    // when:
    auto result = cpu.execute(5, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 5);
}

TEST_F(LSRTest, LSR_Absolute_CorrectCycles)
{
    // given:
    mem[0x8000] = static_cast<u8>(Opcode::LSR_ABS);
    mem[0x8001] = 0x00;
    mem[0x8002] = 0x20;
    mem[0x2000] = 0x01;

    // when:
    auto result = cpu.execute(6, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 6);
}

// ============================================================================
// LSR vs ASL Comparison
// ============================================================================

TEST_F(LSRTest, LSR_OppositeOfASL)
{
    // given: LSR shifts right, ASL shifts left
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0b00001000;  // 8
    mem[0x8002] = static_cast<u8>(Opcode::ASL_A);
    mem[0x8003] = static_cast<u8>(Opcode::LSR_A);

    // when:
    auto result = cpu.execute(6, mem);  // 2 + 2 + 2

    // then: Should be back to original value
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0b00001000);
}

// ============================================================================
// Real-World Scenarios
// ============================================================================

TEST_F(LSRTest, LSR_DivideByTwo)
{
    // given: Dividing 100 by 2
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 100;
    mem[0x8002] = static_cast<u8>(Opcode::LSR_A);

    // when:
    auto result = cpu.execute(4, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 50);
    EXPECT_FALSE(cpu.get_flags().carry);  // 100 is even
}

TEST_F(LSRTest, LSR_DivideOddNumberByTwo)
{
    // given: Dividing 101 by 2 (truncates to 50)
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 101;
    mem[0x8002] = static_cast<u8>(Opcode::LSR_A);

    // when:
    auto result = cpu.execute(4, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 50);
    EXPECT_TRUE(cpu.get_flags().carry);  // Lost the .5 (odd number)
}

TEST_F(LSRTest, LSR_ExtractBit0)
{
    // given: Check if number is odd/even
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0b10101011;  // Odd number
    mem[0x8002] = static_cast<u8>(Opcode::LSR_A);

    // when:
    auto result = cpu.execute(4, mem);

    // then: Carry flag tells us if original was odd
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(cpu.get_flags().carry);  // Was odd
}

TEST_F(LSRTest, LSR_DivideBy16)
{
    // given: Divide by 16 using 4 shifts
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 160;  // 160 / 16 = 10
    mem[0x8002] = static_cast<u8>(Opcode::LSR_A);
    mem[0x8003] = static_cast<u8>(Opcode::LSR_A);
    mem[0x8004] = static_cast<u8>(Opcode::LSR_A);
    mem[0x8005] = static_cast<u8>(Opcode::LSR_A);

    // when:
    auto result = cpu.execute(10, mem);  // 2 + 2 + 2 + 2 + 2

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 10);
}

TEST_F(LSRTest, LSR_ClearHighNibble)
{
    // given: Extract low nibble by clearing high bits
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0xAB;
    mem[0x8002] = static_cast<u8>(Opcode::LSR_A);
    mem[0x8003] = static_cast<u8>(Opcode::LSR_A);
    mem[0x8004] = static_cast<u8>(Opcode::LSR_A);
    mem[0x8005] = static_cast<u8>(Opcode::LSR_A);

    // when:
    auto result = cpu.execute(10, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0x0A);  // High nibble shifted out
}