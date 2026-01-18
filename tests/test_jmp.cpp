#include <gtest/gtest.h>
#include "cpu6502/cpu.hpp"
#include "cpu6502/memory.hpp"
#include "cpu6502/opcodes.hpp"

using namespace cpu6502;

class JMPTest : public ::testing::Test
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
// JMP Absolute Tests
// ============================================================================

TEST_F(JMPTest, JMP_Absolute_BasicJump)
{
    // given: Jump to $4242
    mem[0x8000] = static_cast<u8>(Opcode::JMP_ABS);
    mem[0x8001] = 0x42;  // Low byte
    mem[0x8002] = 0x42;  // High byte
    mem[0x4242] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x4243] = 0xAA;

    // when: Execute JMP and then the instruction at target
    auto result = cpu.execute(5, mem);  // 3 cycles JMP + 2 cycles LDA

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_pc(), 0x4244);  // PC should be after LDA instruction
    EXPECT_EQ(cpu.get_a(), 0xAA);     // LDA should have executed
}

TEST_F(JMPTest, JMP_Absolute_JumpToZeroPage)
{
    // given: Jump to zero page
    mem[0x8000] = static_cast<u8>(Opcode::JMP_ABS);
    mem[0x8001] = 0x42;  // Low byte
    mem[0x8002] = 0x00;  // High byte ($0042)
    mem[0x0042] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x0043] = 0x55;

    // when:
    auto result = cpu.execute(5, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_pc(), 0x0044);
    EXPECT_EQ(cpu.get_a(), 0x55);
}

TEST_F(JMPTest, JMP_Absolute_JumpForward)
{
    // given: Jump forward in memory
    mem[0x8000] = static_cast<u8>(Opcode::JMP_ABS);
    mem[0x8001] = 0x00;  // Low byte
    mem[0x8002] = 0x90;  // High byte ($9000)
    mem[0x9000] = static_cast<u8>(Opcode::LDX_IM);
    mem[0x9001] = 0x77;

    // when:
    auto result = cpu.execute(5, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_pc(), 0x9002);
    EXPECT_EQ(cpu.get_x(), 0x77);
}

TEST_F(JMPTest, JMP_Absolute_InfiniteLoop)
{
    // given: JMP to itself (infinite loop)
    mem[0x8000] = static_cast<u8>(Opcode::JMP_ABS);
    mem[0x8001] = 0x00;  // Low byte
    mem[0x8002] = 0x80;  // High byte ($8000) - jump to self

    // when: Execute just one JMP
    auto result = cpu.execute(3, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_pc(), 0x8000);  // Should be back at start
}

TEST_F(JMPTest, JMP_Absolute_CorrectCycles)
{
    // given:
    mem[0x8000] = static_cast<u8>(Opcode::JMP_ABS);
    mem[0x8001] = 0x00;
    mem[0x8002] = 0x42;

    // when:
    auto result = cpu.execute(3, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 3);  // JMP Absolute takes 3 cycles
}

// ============================================================================
// JMP Indirect Tests
// ============================================================================

TEST_F(JMPTest, JMP_Indirect_BasicOperation)
{
    // given: Jump indirectly through $2000
    mem[0x8000] = static_cast<u8>(Opcode::JMP_IND);
    mem[0x8001] = 0x00;  // Pointer low byte
    mem[0x8002] = 0x20;  // Pointer high byte ($2000)
    
    // Target address stored at $2000
    mem[0x2000] = 0x42;  // Target low byte
    mem[0x2001] = 0x42;  // Target high byte ($4242)
    
    // Instruction at target
    mem[0x4242] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x4243] = 0xBB;

    // when:
    auto result = cpu.execute(7, mem);  // 5 cycles JMP + 2 cycles LDA

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_pc(), 0x4244);
    EXPECT_EQ(cpu.get_a(), 0xBB);
}

TEST_F(JMPTest, JMP_Indirect_PageBoundaryBug)
{
    // given: The famous 6502 JMP indirect bug
    // When pointer is at end of page (e.g., $20FF), the high byte
    // is read from $2000 instead of $2100
    
    mem[0x8000] = static_cast<u8>(Opcode::JMP_IND);
    mem[0x8001] = 0xFF;  // Pointer low byte
    mem[0x8002] = 0x20;  // Pointer high byte ($20FF)
    
    // Target address bytes
    mem[0x20FF] = 0x42;  // Low byte at $20FF
    mem[0x2000] = 0x42;  // High byte wraps to $2000 (bug!)
    mem[0x2100] = 0x99;  // This would be used if bug didn't exist
    
    // Expected target: $4242 (not $9942)
    mem[0x4242] = static_cast<u8>(Opcode::LDY_IM);
    mem[0x4243] = 0xCC;

    // when:
    auto result = cpu.execute(7, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_pc(), 0x4244);
    EXPECT_EQ(cpu.get_y(), 0xCC);
}

TEST_F(JMPTest, JMP_Indirect_NormalCrossPageRead)
{
    // given: Pointer NOT at page boundary - should work normally
    mem[0x8000] = static_cast<u8>(Opcode::JMP_IND);
    mem[0x8001] = 0xFE;  // Pointer low byte
    mem[0x8002] = 0x20;  // Pointer high byte ($20FE)
    
    // Target address
    mem[0x20FE] = 0x00;  // Low byte
    mem[0x20FF] = 0x30;  // High byte ($3000)
    
    mem[0x3000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x3001] = 0xDD;

    // when:
    auto result = cpu.execute(7, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_pc(), 0x3002);
    EXPECT_EQ(cpu.get_a(), 0xDD);
}

TEST_F(JMPTest, JMP_Indirect_PointerInZeroPage)
{
    // given: Pointer in zero page
    mem[0x8000] = static_cast<u8>(Opcode::JMP_IND);
    mem[0x8001] = 0x10;  // Pointer low byte
    mem[0x8002] = 0x00;  // Pointer high byte ($0010)
    
    // Target address in zero page
    mem[0x0010] = 0x00;  // Low byte
    mem[0x0011] = 0x50;  // High byte ($5000)
    
    mem[0x5000] = static_cast<u8>(Opcode::LDX_IM);
    mem[0x5001] = 0xEE;

    // when:
    auto result = cpu.execute(7, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_pc(), 0x5002);
    EXPECT_EQ(cpu.get_x(), 0xEE);
}

TEST_F(JMPTest, JMP_Indirect_CorrectCycles)
{
    // given:
    mem[0x8000] = static_cast<u8>(Opcode::JMP_IND);
    mem[0x8001] = 0x00;
    mem[0x8002] = 0x20;
    mem[0x2000] = 0x00;
    mem[0x2001] = 0x42;

    // when:
    auto result = cpu.execute(5, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 5);  // JMP Indirect takes 5 cycles
}

// ============================================================================
// JMP vs JSR Comparison Tests
// ============================================================================

TEST_F(JMPTest, JMP_DoesNotPushToStack)
{
    // given:
    u8 initial_sp = cpu.get_sp();
    
    mem[0x8000] = static_cast<u8>(Opcode::JMP_ABS);
    mem[0x8001] = 0x42;
    mem[0x8002] = 0x42;

    // when:
    auto result = cpu.execute(3, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_sp(), initial_sp);  // Stack pointer unchanged
}

TEST_F(JMPTest, JMP_CannotReturn)
{
    // given: JMP doesn't save return address
    mem[0x8000] = static_cast<u8>(Opcode::JMP_ABS);
    mem[0x8001] = 0x42;
    mem[0x8002] = 0x42;
    mem[0x4242] = static_cast<u8>(Opcode::RTS);  // RTS will fail/corrupt

    // when: Execute JMP
    auto result = cpu.execute(3, mem);

    // then: JMP succeeds, but RTS would pull garbage from stack
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_pc(), 0x4242);
}

// ============================================================================
// Real-World Scenario Tests
// ============================================================================

TEST_F(JMPTest, JMP_JumpTable)
{
    // given: Implement a simple jump table
    // Jump to address determined by value in accumulator
    
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x01;  // Index 1
    
    // Jump indirectly through table
    mem[0x8002] = static_cast<u8>(Opcode::JMP_IND);
    mem[0x8003] = 0x10;  // Table base
    mem[0x8004] = 0x30;  // $3010
    
    // Jump table at $3010
    mem[0x3010] = 0x00;  // Entry 0: $5000
    mem[0x3011] = 0x50;
    mem[0x3012] = 0x00;  // Entry 1: $6000
    mem[0x3013] = 0x60;
    
    // Code at entry 1
    mem[0x6000] = static_cast<u8>(Opcode::LDX_IM);
    mem[0x6001] = 0xFF;

    // when: Jump to table entry 0 (would need indexing logic in real code)
    auto result = cpu.execute(7, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    // This demonstrates the pattern, though real jump tables
    // would need indexed addressing or calculated offsets
}

TEST_F(JMPTest, JMP_StateTransition)
{
    // given: State machine using JMP
    mem[0x8000] = static_cast<u8>(Opcode::JMP_ABS);
    mem[0x8001] = 0x00;  // State 1
    mem[0x8002] = 0x81;  // $8100
    
    // State 1: Do something and jump to state 2
    mem[0x8100] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8101] = 0x01;
    mem[0x8102] = static_cast<u8>(Opcode::JMP_ABS);
    mem[0x8103] = 0x00;  // State 2
    mem[0x8104] = 0x82;  // $8200
    
    // State 2: Do something else
    mem[0x8200] = static_cast<u8>(Opcode::LDX_IM);
    mem[0x8201] = 0x02;

    // when: Execute state transitions
    auto result = cpu.execute(10, mem);  // 3 + 2 + 3 + 2

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0x01);  // State 1 executed
    EXPECT_EQ(cpu.get_x(), 0x02);  // State 2 executed
}

// ============================================================================
// Edge Cases
// ============================================================================

TEST_F(JMPTest, JMP_ToHighMemory)
{
    // given: Jump to highest addressable memory
    mem[0x8000] = static_cast<u8>(Opcode::JMP_ABS);
    mem[0x8001] = 0xFD;
    mem[0x8002] = 0xFF;  // $FFFD
    
    mem[0xFFFD] = static_cast<u8>(Opcode::LDA_IM);
    mem[0xFFFE] = 0x42;

    // when:
    auto result = cpu.execute(5, mem);

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0x42);
}

TEST_F(JMPTest, JMP_ConsecutiveJumps)
{
    // given: Multiple JMP instructions
    mem[0x8000] = static_cast<u8>(Opcode::JMP_ABS);
    mem[0x8001] = 0x00;
    mem[0x8002] = 0x81;  // Jump to $8100
    
    mem[0x8100] = static_cast<u8>(Opcode::JMP_ABS);
    mem[0x8101] = 0x00;
    mem[0x8102] = 0x82;  // Jump to $8200
    
    mem[0x8200] = static_cast<u8>(Opcode::JMP_ABS);
    mem[0x8201] = 0x00;
    mem[0x8202] = 0x83;  // Jump to $8300
    
    mem[0x8300] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8301] = 0x99;

    // when:
    auto result = cpu.execute(11, mem);  // 3 + 3 + 3 + 2

    // then:
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_pc(), 0x8302);
    EXPECT_EQ(cpu.get_a(), 0x99);
}