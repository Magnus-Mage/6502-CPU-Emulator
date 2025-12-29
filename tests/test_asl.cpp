#include <gtest/gtest.h>
#include "cpu6502/cpu.hpp"
#include "cpu6502/memory.hpp"
#include "cpu6502/opcodes.hpp"

using namespace cpu6502;

class ASLTest : public ::testing::Test {
protected:
    Memory mem;
    CPU cpu;

    void SetUp() override {
        // Program reset vector to point to $8000
        mem[0xFFFC] = 0x00;
        mem[0xFFFD] = 0x80;
        cpu.reset(mem);
    }
};

// ============================================================================
// ASL Accumulator Tests
// ============================================================================

TEST_F(ASLTest, ASL_Accumulator_ShiftsLeft) {
    // given:
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0b00000101;  // 5
    mem[0x8002] = static_cast<u8>(Opcode::ASL_A);
    
    // when:
    auto result = cpu.execute(4, mem);  // 2 cycles LDA + 2 cycles ASL
    
    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0b00001010);  // 10
    EXPECT_FALSE(cpu.get_flags().carry);
    EXPECT_FALSE(cpu.get_flags().zero);
    EXPECT_FALSE(cpu.get_flags().negative);
}

TEST_F(ASLTest, ASL_Accumulator_SetsCarryFlag) {
    // given:
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0b10000001;  // Bit 7 set
    mem[0x8002] = static_cast<u8>(Opcode::ASL_A);
    
    // when:
    auto result = cpu.execute(4, mem);
    
    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0b00000010);
    EXPECT_TRUE(cpu.get_flags().carry);  // Bit 7 shifted into carry
    EXPECT_FALSE(cpu.get_flags().zero);
    EXPECT_FALSE(cpu.get_flags().negative);
}

TEST_F(ASLTest, ASL_Accumulator_SetsZeroFlag) {
    // given:
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0b10000000;  // Will shift to 0
    mem[0x8002] = static_cast<u8>(Opcode::ASL_A);
    
    // when:
    auto result = cpu.execute(4, mem);
    
    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0);
    EXPECT_TRUE(cpu.get_flags().carry);
    EXPECT_TRUE(cpu.get_flags().zero);
    EXPECT_FALSE(cpu.get_flags().negative);
}

TEST_F(ASLTest, ASL_Accumulator_SetsNegativeFlag) {
    // given:
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0b01000000;  // Bit 6 set, will become bit 7
    mem[0x8002] = static_cast<u8>(Opcode::ASL_A);
    
    // when:
    auto result = cpu.execute(4, mem);
    
    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0b10000000);
    EXPECT_FALSE(cpu.get_flags().carry);
    EXPECT_FALSE(cpu.get_flags().zero);
    EXPECT_TRUE(cpu.get_flags().negative);
}

// ============================================================================
// ASL Zero Page Tests
// ============================================================================

TEST_F(ASLTest, ASL_ZeroPage_ShiftsMemory) {
    // given:
    mem[0x8000] = static_cast<u8>(Opcode::ASL_ZP);
    mem[0x8001] = 0x42;  // Zero page address
    mem[0x0042] = 0b00000011;  // Value to shift
    
    // when:
    auto result = cpu.execute(5, mem);  // 5 cycles for ASL ZP
    
    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(mem[0x0042], 0b00000110);
    EXPECT_FALSE(cpu.get_flags().carry);
    EXPECT_FALSE(cpu.get_flags().zero);
    EXPECT_FALSE(cpu.get_flags().negative);
}

TEST_F(ASLTest, ASL_ZeroPage_UpdatesFlags) {
    // given:
    mem[0x8000] = static_cast<u8>(Opcode::ASL_ZP);
    mem[0x8001] = 0x10;
    mem[0x0010] = 0b11000000;
    
    // when:
    auto result = cpu.execute(5, mem);
    
    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(mem[0x0010], 0b10000000);
    EXPECT_TRUE(cpu.get_flags().carry);   // Bit 7 was set
    EXPECT_FALSE(cpu.get_flags().zero);
    EXPECT_TRUE(cpu.get_flags().negative); // Result bit 7 is set
}

// ============================================================================
// ASL Zero Page,X Tests
// ============================================================================

TEST_F(ASLTest, ASL_ZeroPageX_ShiftsMemoryWithIndex) {
    // given:
    cpu.set_x(0x05);
    mem[0x8000] = static_cast<u8>(Opcode::ASL_ZPX);
    mem[0x8001] = 0x10;  // Base address
    mem[0x0015] = 0b00001111;  // Address 0x10 + 0x05
    
    // when:
    auto result = cpu.execute(6, mem);  // 6 cycles for ASL ZP,X
    
    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(mem[0x0015], 0b00011110);
    EXPECT_FALSE(cpu.get_flags().carry);
}

TEST_F(ASLTest, ASL_ZeroPageX_WrapsAround) {
    // given:
    cpu.set_x(0xFF);
    mem[0x8000] = static_cast<u8>(Opcode::ASL_ZPX);
    mem[0x8001] = 0x10;  // 0x10 + 0xFF = 0x0F (wraps in zero page)
    mem[0x000F] = 0b00000001;
    
    // when:
    auto result = cpu.execute(6, mem);
    
    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(mem[0x000F], 0b00000010);
}

// ============================================================================
// ASL Absolute Tests
// ============================================================================

TEST_F(ASLTest, ASL_Absolute_ShiftsMemory) {
    // given:
    mem[0x8000] = static_cast<u8>(Opcode::ASL_ABS);
    mem[0x8001] = 0x00;  // Low byte of address
    mem[0x8002] = 0x20;  // High byte ($2000)
    mem[0x2000] = 0b00110011;
    
    // when:
    auto result = cpu.execute(6, mem);  // 6 cycles for ASL Absolute
    
    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(mem[0x2000], 0b01100110);
    EXPECT_FALSE(cpu.get_flags().carry);
}

// ============================================================================
// ASL Absolute,X Tests
// ============================================================================

TEST_F(ASLTest, ASL_AbsoluteX_ShiftsMemoryWithIndex) {
    // given:
    cpu.set_x(0x10);
    mem[0x8000] = static_cast<u8>(Opcode::ASL_ABSX);
    mem[0x8001] = 0x00;
    mem[0x8002] = 0x20;  // $2000
    mem[0x2010] = 0b00000111;  // $2000 + $10
    
    // when:
    auto result = cpu.execute(7, mem);  // 7 cycles for ASL Absolute,X
    
    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(mem[0x2010], 0b00001110);
}

TEST_F(ASLTest, ASL_AbsoluteX_WritesToCorrectAddress) {
    // given:
    cpu.set_x(0x05);
    mem[0x8000] = static_cast<u8>(Opcode::ASL_ABSX);
    mem[0x8001] = 0xFF;
    mem[0x8002] = 0x20;  // $20FF
    mem[0x2104] = 0b01010101;  // $20FF + $05 = $2104
    
    // when:
    auto result = cpu.execute(7, mem);
    
    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(mem[0x2104], 0b10101010);  // Shifted at correct address
    EXPECT_FALSE(cpu.get_flags().carry);
    EXPECT_TRUE(cpu.get_flags().negative);
}

// ============================================================================
// Edge Case Tests
// ============================================================================

TEST_F(ASLTest, ASL_MultipleShifts) {
    // given: Shift multiple times
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0b00000001;  // 1
    mem[0x8002] = static_cast<u8>(Opcode::ASL_A);
    mem[0x8003] = static_cast<u8>(Opcode::ASL_A);
    mem[0x8004] = static_cast<u8>(Opcode::ASL_A);
    
    // when:
    auto result = cpu.execute(8, mem);  // 2 + 2 + 2 + 2
    
    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0b00001000);  // 1 << 3 = 8
}

TEST_F(ASLTest, ASL_AllBitsSet) {
    // given:
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0xFF;
    mem[0x8002] = static_cast<u8>(Opcode::ASL_A);
    
    // when:
    auto result = cpu.execute(4, mem);
    
    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0xFE);
    EXPECT_TRUE(cpu.get_flags().carry);
    EXPECT_FALSE(cpu.get_flags().zero);
    EXPECT_TRUE(cpu.get_flags().negative);
}

// ============================================================================
// Cycle Count Tests
// ============================================================================

TEST_F(ASLTest, ASL_Accumulator_CorrectCycles) {
    mem[0x8000] = static_cast<u8>(Opcode::ASL_A);
    
    auto result = cpu.execute(2, mem);
    
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 2);
}

TEST_F(ASLTest, ASL_ZeroPage_CorrectCycles) {
    mem[0x8000] = static_cast<u8>(Opcode::ASL_ZP);
    mem[0x8001] = 0x10;
    mem[0x0010] = 0x01;
    
    auto result = cpu.execute(5, mem);
    
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 5);
}

TEST_F(ASLTest, ASL_Absolute_CorrectCycles) {
    mem[0x8000] = static_cast<u8>(Opcode::ASL_ABS);
    mem[0x8001] = 0x00;
    mem[0x8002] = 0x20;
    mem[0x2000] = 0x01;
    
    auto result = cpu.execute(6, mem);
    
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 6);
}
