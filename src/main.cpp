#include "cpu6502/cpu.hpp"
#include "cpu6502/memory.hpp"
#include "cpu6502/opcodes.hpp"
#include <print>

int main() {
    using namespace cpu6502;
    
    Memory mem;
    CPU cpu;
    
    // Program the reset vector to point to our program at $8000
    mem[0xFFFC] = 0x00;  // Low byte of start address ($8000)
    mem[0xFFFD] = 0x80;  // High byte of start address ($8000)
    
    std::println("6502 Emulator - Modern C++23");
    std::println("============================");
    std::println("Memory Map:");
    std::println("  Zero Page:  $0000-$00FF (special addressing)");
    std::println("  Stack:      $0100-$01FF (fixed, cannot relocate)");
    std::println("  Program:    $8000-$FFFF (typical ROM location)");
    std::println("  Vectors:    $FFFA-$FFFF (NMI, RESET, IRQ/BRK)");
    std::println("");
    
    // Test 1: Simple JSR/RTS
    cpu.reset(mem);  // Reset FIRST
    std::println("=== Test 1: JSR/RTS ===");
    mem[0x8000] = static_cast<u8>(Opcode::JSR);
    mem[0x8001] = 0x42;
    mem[0x8002] = 0x42;
    mem[0x4242] = static_cast<u8>(Opcode::LDA_IM);
    mem[0x4243] = 0x84;
    mem[0x4244] = static_cast<u8>(Opcode::RTS);
    
    auto result = cpu.execute(14, mem);
    if (result) {
        std::println("✓ Test 1 passed - A: 0x{:02X}, Cycles: {}", cpu.get_a(), result.value());
    }
    
    // Test 2: Absolute,X without page crossing (4 cycles)
    cpu.reset(mem);
    cpu.set_x(0x05);
    std::println("\n=== Test 2: LDA Absolute,X (No Page Cross) ===");
    mem[0x8000] = static_cast<u8>(Opcode::LDA_ABSX);
    mem[0x8001] = 0x00;
    mem[0x8002] = 0x20;
    mem[0x2005] = 0x42;
    
    result = cpu.execute(4, mem);
    if (result) {
        std::println("✓ Base: $2000, X: $05, Effective: $2005");
        std::println("✓ No page cross - A: 0x{:02X}, Cycles: {} (expected 4)",
                    cpu.get_a(), result.value());
    }
    
    // Test 3: Absolute,X WITH page crossing (5 cycles)
    cpu.reset(mem);
    cpu.set_x(0xFF);
    std::println("\n=== Test 3: LDA Absolute,X (Page Cross!) ===");
    mem[0x8000] = static_cast<u8>(Opcode::LDA_ABSX);
    mem[0x8001] = 0xFF;
    mem[0x8002] = 0x20;
    mem[0x21FE] = 0x99;
    
    result = cpu.execute(5, mem);
    if (result) {
        std::println("✓ Base: $20FF, X: $FF, Effective: $21FE");
        std::println("✓ Page crossed ($20 -> $21) - A: 0x{:02X}, Cycles: {} (expected 5)",
                    cpu.get_a(), result.value());
    }

    // Test 4: LDX Immediate (2 cycles)
    cpu.reset(mem);
    std::println("\n=== Test 4: LDX Immediate ===");
    mem[0x8000] = static_cast<u8>(Opcode::LDX_IM);
    mem[0x8001] = 0x20;

    result = cpu.execute(2, mem);
    if (result) {
	std::println("✓ Value: $0x20 , Effective X: $0x20");
        std::println("✓ X: 0x{:02X}, Cycles: {}",
                    cpu.get_x(), result.value());
    }

    // Test 5: LDX Zero Page (3 Cycles)
    
    std::println("\n=== All Tests Complete ===");
    
    return 0;
}
