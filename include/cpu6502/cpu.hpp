#pragma once

#include "memory.hpp"
#include "types.hpp"
#include "error.hpp"
#include "status_flags.hpp"
#include <expected>
#include <string>

namespace cpu6502 
{

/**
* @type class
* @brief CPU class that would include the main cpu core subsystem
*/
class CPU 
{

public:
   // Constants based on 6502 Architecture
   // Stack is fixed at $0100-$0x01FF (second page, cannot be reallocated)
   static constexpr u16 STACK_PAGE   = 0x0100;

   // Reset vector at $FFFC-$FFFD (power-on reset location)
   static constexpr u16 RESET_VECTOR = 0xFFFC;

   // Stack pointer is 8 bit offset from $0100 , initialised to $FF
   // Full stack address = $0100 + SP, so initial stack top is $01FF
   static constexpr u8 INITIAL_SP   = 0xFF;

   // Interrupt vectors 
   static constexpr NMI_VECTOR = 0xFFFA;    // non-maskable interrupt
   static constexpr IRQ_VECTOR = 0xFFFE;    // IRQ/BRK interrupt

   constexpr CPU() = default();

   // Lifecycle
   constexpr void reset(Memory memory) noexcept;

   // Execution blocks
   [[nodiscard]] auto execute(i32 cycles, Memory& memory)
       -> std::expected<i32, EmulatorError>;

   // Getter and setters for debugging
   [[nodiscard]] constexpr u16          get_pc()     const noexcept { return pc_; }
   [[nodiscard]] constexpr u8           get_sp()     const noexcept { return sp_; }
   [[nodiscard]] constexpr u8           get_a()      const noexcept { return a_; }
   [[nodiscard]] constexpr u8           get_x()      const noexcept { return x_; }
   [[nodiscard]] constexpr u8           get_y()      const noexcept { return y_; }
   [[nodiscard]] constexpr StatusFlags  get_flags()  const noexcept { return flags_; }

   constexpr void set_x(u8 value) noexcept { x_ = value; }
   constexpr void set_y(u8 value) noexcept { y_ = value; }

private:
   u16          pc_{};          // Program Counter
   u8           sp_{};          // Stack Pointer
   u8           a_{};           // Accumulator
   u8           x_{};           // X register
   u8           y_{};           // Y register
   StatusFlags  flags_{};       

    // Core operations
    [[nodiscard]] constexpr auto fetch_byte(i32& cycles, Memory& memory) 
        -> std::expected<u8, EmulatorError>;
    
    [[nodiscard]] constexpr auto fetch_word(i32& cycles, Memory& memory) 
        -> std::expected<u16, EmulatorError>;
    
    [[nodiscard]] constexpr auto read_byte(i32& cycles, u16 address, Memory& memory) 
        -> std::expected<u8, EmulatorError>;
    
    constexpr auto push_byte(i32& cycles, u8 value, Memory& memory) 
        -> std::expected<void, EmulatorError>;
    
    [[nodiscard]] constexpr auto pop_byte(i32& cycles, Memory& memory) 
        -> std::expected<u8, EmulatorError>;
    
    // Flag operations
    constexpr void set_zn_flags(u8 value) noexcept;
    constexpr void load_accumulator(u8 value) noexcept;
    
    // Instruction execution
    [[nodiscard]] constexpr auto fetch_and_execute(i32& cycles, Memory& memory) 
        -> std::expected<void, EmulatorError>;
    
    // Individual instruction implementations
    [[nodiscard]] constexpr auto execute_lda_immediate(i32& cycles, Memory& memory) 
        -> std::expected<void, EmulatorError>;
    
    [[nodiscard]] constexpr auto execute_lda_zero_page(i32& cycles, Memory& memory) 
        -> std::expected<void, EmulatorError>;
    
    [[nodiscard]] constexpr auto execute_lda_zero_page_x(i32& cycles, Memory& memory) 
        -> std::expected<void, EmulatorError>;
    
    [[nodiscard]] constexpr auto execute_lda_absolute(i32& cycles, Memory& memory) 
        -> std::expected<void, EmulatorError>;
    
    [[nodiscard]] constexpr auto execute_jsr(i32& cycles, Memory& memory) 
        -> std::expected<void, EmulatorError>;
    
    [[nodiscard]] constexpr auto execute_rts(i32& cycles, Memory& memory) 
        -> std::expected<void, EmulatorError>;

};


} // namespace cpu6502
