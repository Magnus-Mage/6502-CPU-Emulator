#include <gtest/gtest.h>
#include "cpu6502/cpu.hpp"
#include "cpu6502/memory.hpp"
#include "cpu6502/opcodes.hpp"

using namespace cpu6502;

class LDXYTest : public ::testing::Test {
 protected:
    Memory mem;
    CPU    cpu;

    void SetUp() override {
        mem[0xFFFC] = 0x00;
        mem[0xFFFD] = 0x80;
        cpu.reset(mem);
    }
};

TEST_F(LDXYTest, LDX_Immediate_LoadsValue) {
    mem[0x8000] = static_cast<u8>(Opcode::LDX_IM);
    mem[0x8001] = 0x42;

    auto result = cpu.execute(2, mem);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_x(), 0x42);
}

TEST_F(LDXYTest, LDY_Immediate_LoadsValue) {
    mem[0x8000] = static_cast<u8>(Opcode::LDY_IM);
    mem[0x8001] = 0x84;

    auto result = cpu.execute(2, mem);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_y(), 0x84);
}
