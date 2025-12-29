#include <gtest/gtest.h>
#include "cpu6502/cpu.hpp"
#include "cpu6502/memory.hpp"
#include "cpu6502/opcodes.hpp"

using namespace cpu6502;

class LDATest : public ::testing::Test {
 protected:
    Memory mem;
    CPU    cpu;

    void SetUp() override {
        mem[0xFFFC] = 0x00;
        mem[0xFFFD] = 0x80;
        cpu.reset(mem);
    }
};

TEST_F(LDATest, LDA_Immediate_LoadsValue) {
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x42;

    auto result = cpu.execute(2, mem);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0x42);
    EXPECT_FALSE(cpu.get_flags().zero);
    EXPECT_FALSE(cpu.get_flags().negative);
}

TEST_F(LDATest, LDA_Immediate_SetsZeroFlag) {
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x00;

    auto result = cpu.execute(2, mem);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0x00);
    EXPECT_TRUE(cpu.get_flags().zero);
}

TEST_F(LDATest, LDA_Immediate_SetsNegativeFlag) {
    mem[0x8000] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x8001] = 0x80;

    auto result = cpu.execute(2, mem);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0x80);
    EXPECT_TRUE(cpu.get_flags().negative);
}
