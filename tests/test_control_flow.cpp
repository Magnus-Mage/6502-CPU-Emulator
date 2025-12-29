#include <gtest/gtest.h>
#include "cpu6502/cpu.hpp"
#include "cpu6502/memory.hpp"
#include "cpu6502/opcodes.hpp"

using namespace cpu6502;

class ControlFlowTest : public ::testing::Test {
 protected:
    Memory mem;
    CPU    cpu;

    void SetUp() override {
        mem[0xFFFC] = 0x00;
        mem[0xFFFD] = 0x80;
        cpu.reset(mem);
    }
};

TEST_F(ControlFlowTest, JSR_RTS_BasicOperation) {
    mem[0x8000] = static_cast<u8>(Opcode::JSR);
    mem[0x8001] = 0x42;
    mem[0x8002] = 0x42;
    mem[0x4242] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x4243] = 0x84;
    mem[0x4244] = static_cast<u8>(Opcode::RTS);

    auto result = cpu.execute(14, mem);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(cpu.get_a(), 0x84);
}
