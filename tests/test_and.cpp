#include <gtest/gtest.h>
#include "cpu6502/cpu.hpp"
#include "cpu6502/memory.hpp"
#include "cpu6502/opcodes.hpp"

using namespace cpu6502;

class ANDTest : public ::testing::Test {
 protected:
    Memory mem;
    CPU    cpu;

    void SetUp() override {
        mem[0xFFFC] = 0x00;
        mem[0xFFFD] = 0x80;
        cpu.reset(mem);
    }
};

TEST_F(ANDTest, AND_Immediate_BasicOperation) {
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0xFF;
    mem[0x8002] = static_cast<u8>(Opcode::AND_IM);
    mem[0x8003] = 0x0F;

    auto result = cpu.execute(4, mem);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0x0F);
}

TEST_F(ANDTest, AND_Immediate_SetsZeroFlag) {
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0xF0;
    mem[0x8002] = static_cast<u8>(Opcode::AND_IM);
    mem[0x8003] = 0x0F;

    auto result = cpu.execute(4, mem);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0x00);
    EXPECT_TRUE(cpu.get_flags().zero);
}
