#include <gtest/gtest.h>
#include "cpu6502/cpu.hpp"
#include "cpu6502/memory.hpp"
#include "cpu6502/opcodes.hpp"

using namespace cpu6502;

class StackOpsTest : public ::testing::Test
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
// NOP Tests
// ============================================================================

TEST_F(StackOpsTest, NOP_DoesNothing)
{
    // given:
    u8 initial_a  = cpu.get_a();
    u8 initial_x  = cpu.get_x();
    u8 initial_y  = cpu.get_y();
    u8 initial_sp = cpu.get_sp();
    auto initial_flags = cpu.get_flags();

    mem[0x8000] = static_cast<u8>(Opcode::NOP);

    // when:
    auto result = cpu.execute(2, mem);

    // then: Nothing changed except PC
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), initial_a);
    EXPECT_EQ(cpu.get_x(), initial_x);
    EXPECT_EQ(cpu.get_y(), initial_y);
    EXPECT_EQ(cpu.get_sp(), initial_sp);
    EXPECT_EQ(cpu.get_flags().carry, initial_flags.carry);
    EXPECT_EQ(cpu.get_flags().zero, initial_flags.zero);
}

TEST_F(StackOpsTest, NOP_CorrectCycles)
{
    // given:
    mem[0x8000] = static_cast<u8>(Opcode::NOP);

    // when:
    auto result = cpu.execute(2, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 2);
}

TEST_F(StackOpsTest, NOP_MultipleNOPs)
{
    // given: Multiple NOPs in sequence
    mem[0x8000] = static_cast<u8>(Opcode::NOP);
    mem[0x8001] = static_cast<u8>(Opcode::NOP);
    mem[0x8002] = static_cast<u8>(Opcode::NOP);

    // when:
    auto result = cpu.execute(6, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 6);
}

// ============================================================================
// PHA Tests
// ============================================================================

TEST_F(StackOpsTest, PHA_PushesAccumulatorToStack)
{
    // given:
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x42;
    mem[0x8002] = static_cast<u8>(Opcode::PHA);

    u8 initial_sp = cpu.get_sp();

    // when:
    auto result = cpu.execute(5, mem);  // 2 (LDA) + 3 (PHA)

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_sp(), initial_sp - 1);  // SP decremented
    EXPECT_EQ(mem[0x0100 + initial_sp], 0x42);  // Value on stack
}

TEST_F(StackOpsTest, PHA_MultiplePushes)
{
    // given:
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x11;
    mem[0x8002] = static_cast<u8>(Opcode::PHA);
    mem[0x8003] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8004] = 0x22;
    mem[0x8005] = static_cast<u8>(Opcode::PHA);
    mem[0x8006] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8007] = 0x33;
    mem[0x8008] = static_cast<u8>(Opcode::PHA);

    u8 initial_sp = cpu.get_sp();

    // when:
    auto result = cpu.execute(15, mem);  // (2+3) + (2+3) + (2+3)

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_sp(), initial_sp - 3);
    EXPECT_EQ(mem[0x0100 + initial_sp], 0x11);      // First push
    EXPECT_EQ(mem[0x0100 + initial_sp - 1], 0x22);  // Second push
    EXPECT_EQ(mem[0x0100 + initial_sp - 2], 0x33);  // Third push
}

TEST_F(StackOpsTest, PHA_DoesNotAffectAccumulator)
{
    // given:
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x42;
    mem[0x8002] = static_cast<u8>(Opcode::PHA);

    // when:
    auto result = cpu.execute(5, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0x42);  // A unchanged
}

TEST_F(StackOpsTest, PHA_CorrectCycles)
{
    // given:
    mem[0x8000] = static_cast<u8>(Opcode::PHA);

    // when:
    auto result = cpu.execute(3, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 3);
}

// ============================================================================
// PHP Tests
// ============================================================================

TEST_F(StackOpsTest, PHP_PushesStatusToStack)
{
    // given: Set some flags
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x00;  // Sets zero flag
    mem[0x8002] = static_cast<u8>(Opcode::PHP);

    u8 initial_sp = cpu.get_sp();

    // when:
    auto result = cpu.execute(5, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_sp(), initial_sp - 1);
    
    // Check that status was pushed
    u8 pushed_status = mem[0x0100 + initial_sp];
    EXPECT_TRUE((pushed_status & 0x02) != 0);  // Zero flag set
    EXPECT_TRUE((pushed_status & 0x10) != 0);  // Break flag set (always set by PHP)
    EXPECT_TRUE((pushed_status & 0x20) != 0);  // Bit 5 set (always set)
}

TEST_F(StackOpsTest, PHP_SetsBreakFlag)
{
    // given:
    mem[0x8000] = static_cast<u8>(Opcode::PHP);

    u8 initial_sp = cpu.get_sp();

    // when:
    auto result = cpu.execute(3, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    u8 pushed_status = mem[0x0100 + initial_sp];
    EXPECT_TRUE((pushed_status & 0x10) != 0);  // Break flag (bit 4) is set
}

TEST_F(StackOpsTest, PHP_SetsUnusedBit)
{
    // given:
    mem[0x8000] = static_cast<u8>(Opcode::PHP);

    u8 initial_sp = cpu.get_sp();

    // when:
    auto result = cpu.execute(3, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    u8 pushed_status = mem[0x0100 + initial_sp];
    EXPECT_TRUE((pushed_status & 0x20) != 0);  // Bit 5 is always set
}

TEST_F(StackOpsTest, PHP_CorrectCycles)
{
    // given:
    mem[0x8000] = static_cast<u8>(Opcode::PHP);

    // when:
    auto result = cpu.execute(3, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 3);
}

// ============================================================================
// PLA Tests
// ============================================================================

TEST_F(StackOpsTest, PLA_PullsAccumulatorFromStack)
{
    // given: First push a value
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x42;
    mem[0x8002] = static_cast<u8>(Opcode::PHA);
    mem[0x8003] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8004] = 0x00;  // Clear A
    mem[0x8005] = static_cast<u8>(Opcode::PLA);

    u8 initial_sp = cpu.get_sp();

    // when:
    auto result = cpu.execute(11, mem);  // 2 (LDA) + 3 (PHA) + 2 (LDA) + 4 (PLA)

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0x42);  // Restored from stack
    EXPECT_EQ(cpu.get_sp(), initial_sp);  // SP back to original
}

TEST_F(StackOpsTest, PLA_SetsZeroFlag)
{
    // given: Push 0
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x00;
    mem[0x8002] = static_cast<u8>(Opcode::PHA);
    mem[0x8003] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8004] = 0xFF;  // Set A to non-zero
    mem[0x8005] = static_cast<u8>(Opcode::PLA);

    // when:
    auto result = cpu.execute(11, mem);  // 2 + 3 + 2 + 4

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0x00);
    EXPECT_TRUE(cpu.get_flags().zero);
}

TEST_F(StackOpsTest, PLA_SetsNegativeFlag)
{
    // given: Push negative value
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x80;
    mem[0x8002] = static_cast<u8>(Opcode::PHA);
    mem[0x8003] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8004] = 0x00;
    mem[0x8005] = static_cast<u8>(Opcode::PLA);

    // when:
    auto result = cpu.execute(11, mem);  // 2 + 3 + 2 + 4

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0x80);
    EXPECT_TRUE(cpu.get_flags().negative);
}

TEST_F(StackOpsTest, PLA_CorrectCycles)
{
    // given:
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x42;
    mem[0x8002] = static_cast<u8>(Opcode::PHA);
    mem[0x8003] = static_cast<u8>(Opcode::PLA);

    // when:
    auto result = cpu.execute(9, mem);  // 2 + 3 + 4

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 9);
}

// ============================================================================
// PLP Tests
// ============================================================================

TEST_F(StackOpsTest, PLP_RestoresStatusFromStack)
{
    // given: Set flags, push them, clear flags, restore them
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x00;  // Zero flag set
    mem[0x8002] = static_cast<u8>(Opcode::PHP);
    mem[0x8003] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8004] = 0xFF;  // Clear zero flag
    mem[0x8005] = static_cast<u8>(Opcode::PLP);

    // when:
    auto result = cpu.execute(13, mem);  // 2 + 3 + 2 + 4 + 2 (not 14)

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(cpu.get_flags().zero);  // Zero flag restored
}

TEST_F(StackOpsTest, PLP_ClearsBreakFlag)
{
    // given: PHP sets break flag when pushing
    mem[0x8000] = static_cast<u8>(Opcode::PHP);
    mem[0x8001] = static_cast<u8>(Opcode::PLP);

    // when:
    auto result = cpu.execute(7, mem);  // 3 + 4

    // then: Break flag should be cleared after PLP
    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(cpu.get_flags().brk);
}

TEST_F(StackOpsTest, PLP_RestoresAllFlags)
{
    // given: Set specific flag pattern
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0xFF;  
    mem[0x8002] = static_cast<u8>(Opcode::ADC_IM);
    mem[0x8003] = 0x01;  // Carry
    mem[0x8004] = static_cast<u8>(Opcode::PHP);
    mem[0x8005] = static_cast<u8>(Opcode::CLC);  // Clear carry
    mem[0x8006] = static_cast<u8>(Opcode::PLP);

    // when:
    auto result = cpu.execute(14, mem);  // 2 + 2 + 3 + 2 + 4 + 1

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(cpu.get_flags().carry);  // Restored
}

TEST_F(StackOpsTest, PLP_CorrectCycles)
{
    // given:
    mem[0x8000] = static_cast<u8>(Opcode::PHP);
    mem[0x8001] = static_cast<u8>(Opcode::PLP);

    // when:
    auto result = cpu.execute(7, mem);  // 3 + 4

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 7);
}

// ============================================================================
// Combined Stack Operations Tests
// ============================================================================

TEST_F(StackOpsTest, PHA_PLA_RoundTrip)
{
    // given:
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x42;
    mem[0x8002] = static_cast<u8>(Opcode::PHA);
    mem[0x8003] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8004] = 0x99;
    mem[0x8005] = static_cast<u8>(Opcode::PLA);

    // when:
    auto result = cpu.execute(13, mem);

    // then: Original value restored
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0x42);
}

TEST_F(StackOpsTest, PHP_PLP_RoundTrip)
{
    // given:
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x00;  // Set zero flag
    mem[0x8002] = static_cast<u8>(Opcode::PHP);
    mem[0x8003] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8004] = 0xFF;  // Clear zero flag
    mem[0x8005] = static_cast<u8>(Opcode::PLP);

    // when:
    auto result = cpu.execute(13, mem);  // 2 + 3 + 2 + 4 + 2

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(cpu.get_flags().zero);
}

TEST_F(StackOpsTest, StackPointer_CorrectMovement)
{
    // given:
    u8 initial_sp = cpu.get_sp();
    
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x42;
    mem[0x8002] = static_cast<u8>(Opcode::PHA);  // SP--
    mem[0x8003] = static_cast<u8>(Opcode::PHP);  // SP--
    mem[0x8004] = static_cast<u8>(Opcode::PLP);  // SP++
    mem[0x8005] = static_cast<u8>(Opcode::PLA);  // SP++

    // when:
    auto result = cpu.execute(16, mem);

    // then: SP should be back to initial
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_sp(), initial_sp);
}

TEST_F(StackOpsTest, MultipleValues_LIFO_Order)
{
    // given: Last In First Out
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x11;
    mem[0x8002] = static_cast<u8>(Opcode::PHA);
    mem[0x8003] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8004] = 0x22;
    mem[0x8005] = static_cast<u8>(Opcode::PHA);
    mem[0x8006] = static_cast<u8>(Opcode::PLA);  // Should get 0x22
    mem[0x8007] = static_cast<u8>(Opcode::PHA);  // Push it back
    mem[0x8008] = static_cast<u8>(Opcode::PLA);  // Get 0x22 again
    mem[0x8009] = static_cast<u8>(Opcode::PLA);  // Get 0x11

    // when:
    auto result = cpu.execute(24, mem);

    // then: Should have 0x11 (first pushed, last popped)
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0x11);
}

// ============================================================================
// Real-World Scenarios
// ============================================================================

TEST_F(StackOpsTest, SaveAndRestoreAccumulator)
{
    // given: Common pattern - save A, do work, restore A
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x42;
    mem[0x8002] = static_cast<u8>(Opcode::PHA);  // Save A
    mem[0x8003] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8004] = 0x99;  // Do some work
    mem[0x8005] = static_cast<u8>(Opcode::PLA);  // Restore A

    // when:
    auto result = cpu.execute(11, mem);  // 2 + 3 + 2 + 4

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0x42);
}

TEST_F(StackOpsTest, SaveFlagsBeforeCriticalOperation)
{
    // given: Save flags before operation that might change them
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x00;  // Zero flag set
    mem[0x8002] = static_cast<u8>(Opcode::PHP);  // Save flags
    mem[0x8003] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8004] = 0xFF;  // Change flags
    mem[0x8005] = static_cast<u8>(Opcode::PLP);  // Restore flags

    // when:
    auto result = cpu.execute(13, mem);  // 2 + 3 + 2 + 4 + 2

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(cpu.get_flags().zero);  // Original flags restored
}

TEST_F(StackOpsTest, ParameterPassing)
{
    // given: Push multiple parameters onto stack
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x10;  // First parameter
    mem[0x8002] = static_cast<u8>(Opcode::PHA);
    mem[0x8003] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8004] = 0x20;  // Second parameter
    mem[0x8005] = static_cast<u8>(Opcode::PHA);
    mem[0x8006] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8007] = 0x30;  // Third parameter
    mem[0x8008] = static_cast<u8>(Opcode::PHA);

    u8 initial_sp = cpu.get_sp();

    // when:
    auto result = cpu.execute(15, mem);

    // then: All parameters on stack
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_sp(), initial_sp - 3);
    EXPECT_EQ(mem[0x0100 + initial_sp], 0x10);
    EXPECT_EQ(mem[0x0100 + initial_sp - 1], 0x20);
    EXPECT_EQ(mem[0x0100 + initial_sp - 2], 0x30);
}

TEST_F(StackOpsTest, NOP_ForTiming)
{
    // given: Use NOPs to add delay/timing
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x42;
    mem[0x8002] = static_cast<u8>(Opcode::NOP);
    mem[0x8003] = static_cast<u8>(Opcode::NOP);
    mem[0x8004] = static_cast<u8>(Opcode::NOP);
    mem[0x8005] = static_cast<u8>(Opcode::PHA);

    // when:
    auto result = cpu.execute(11, mem);  // 2 + 2 + 2 + 2 + 3

    // then: Delay added without affecting operation
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 11);
    EXPECT_EQ(cpu.get_a(), 0x42);
}
