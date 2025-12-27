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
   static constexpr u16 NMI_VECTOR = 0xFFFA;    // non-maskable interrupt
   static constexpr u16 IRQ_VECTOR = 0xFFFE;    // IRQ/BRK interrupt

   constexpr CPU() = default;

   // Lifecycle
   constexpr void reset(Memory& memory) noexcept;

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

   // Setters for flags
   constexpr void set_flag_c(bool value)       noexcept { flags_.carry = value; }


   constexpr void clear_flag_c()               noexcept { flags_.carry = false; }

   constexpr void set_flags(StatusFlags flags) noexcept { flags_ = flags; }

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
    constexpr void set_zn_flags(u8 value)     noexcept;
    constexpr void load_accumulator(u8 value) noexcept;
    constexpr void load_x_register(u8 value)  noexcept;
    constexpr void load_y_register(u8 value)  noexcept;
    constexpr void add_with_carry(u8 value)   noexcept;
    constexpr void logical_and(u8 value)      noexcept;

    // Helper for page boundary detection
    [[nodiscard]] static constexpr auto page_crossed(u16 base_addr, u16 effective_addr) noexcept
    {
	    return (base_addr & 0xFF00) != (effective_addr & 0xFF00);
    }
    
    // Instruction execution
    [[nodiscard]] constexpr auto fetch_and_execute(i32& cycles, Memory& memory) 
        -> std::expected<void, EmulatorError>;
    
    // Individual instruction implementations
    // Load Accumulator

    [[nodiscard]] constexpr auto execute_lda_immediate(i32& cycles, Memory& memory) 
        -> std::expected<void, EmulatorError>;
    
    [[nodiscard]] constexpr auto execute_lda_zero_page(i32& cycles, Memory& memory) 
        -> std::expected<void, EmulatorError>;
    
    [[nodiscard]] constexpr auto execute_lda_zero_page_x(i32& cycles, Memory& memory) 
        -> std::expected<void, EmulatorError>;
    
    [[nodiscard]] constexpr auto execute_lda_absolute(i32& cycles, Memory& memory) 
        -> std::expected<void, EmulatorError>;
    
    [[nodiscard]] constexpr auto execute_lda_absolute_x(i32& cycles, Memory& memory)
	    -> std::expected<void, EmulatorError>;

    [[nodiscard]] constexpr auto execute_lda_absolute_y(i32& cycles, Memory& memory)
	    -> std::expected<void, EmulatorError>;
    
    // Control Flow instructions

    [[nodiscard]] constexpr auto execute_jsr(i32& cycles, Memory& memory) 
        -> std::expected<void, EmulatorError>;
    
    [[nodiscard]] constexpr auto execute_rts(i32& cycles, Memory& memory) 
        -> std::expected<void, EmulatorError>;
    
    // Load X Register
 
    [[nodiscard]] constexpr auto execute_ldx_immediate(i32& cycles, Memory& memory)
	-> std::expected<void, EmulatorError>;

    [[nodiscard]] constexpr auto execute_ldx_zero_page(i32& cycles, Memory& memory)
	-> std::expected<void, EmulatorError>;

    [[nodiscard]] constexpr auto execute_ldx_zero_page_y(i32& cycles, Memory& memory)
	-> std::expected<void, EmulatorError>;

    [[nodiscard]] constexpr auto execute_ldx_absolute(i32& cycles, Memory& memory)
	-> std::expected<void, EmulatorError>;

    [[nodiscard]] constexpr auto execute_ldx_absolute_y(i32& cycles, Memory& memory)
	-> std::expected<void, EmulatorError>;

    // Load Y Register
    [[nodiscard]] constexpr auto execute_ldy_immediate(i32& cycles, Memory& memory)
	-> std::expected<void, EmulatorError>;

    [[nodiscard]] constexpr auto execute_ldy_zero_page(i32& cycles, Memory& memory)
	-> std::expected<void, EmulatorError>;

    [[nodiscard]] constexpr auto execute_ldy_zero_page_x(i32& cycles, Memory& memory)
	-> std::expected<void, EmulatorError>;

    [[nodiscard]] constexpr auto execute_ldy_absolute(i32& cycles, Memory& memory)
	-> std::expected<void, EmulatorError>;

    [[nodiscard]] constexpr auto execute_ldy_absolute_x(i32& cycles, Memory& memory)
	-> std::expected<void, EmulatorError>;

    // ADD With Carry

    [[nodiscard]] constexpr auto execute_adc_immediate(i32& cycles, Memory& memory) 
        -> std::expected<void, EmulatorError>;
    
    [[nodiscard]] constexpr auto execute_adc_zero_page(i32& cycles, Memory& memory) 
        -> std::expected<void, EmulatorError>;
    
    [[nodiscard]] constexpr auto execute_adc_zero_page_x(i32& cycles, Memory& memory) 
        -> std::expected<void, EmulatorError>;
    
    [[nodiscard]] constexpr auto execute_adc_absolute(i32& cycles, Memory& memory) 
        -> std::expected<void, EmulatorError>;
    
    [[nodiscard]] constexpr auto execute_adc_absolute_x(i32& cycles, Memory& memory)
	    -> std::expected<void, EmulatorError>;

    [[nodiscard]] constexpr auto execute_adc_absolute_y(i32& cycles, Memory& memory)
	    -> std::expected<void, EmulatorError>;
    
    [[nodiscard]] constexpr auto execute_adc_indirect_x(i32& cycles, Memory& memory)
	    -> std::expected<void, EmulatorError>;

    [[nodiscard]] constexpr auto execute_adc_indirect_y(i32& cycles, Memory& memory)
	    -> std::expected<void, EmulatorError>;

    // LOGICAL AND

    [[nodiscard]] constexpr auto execute_and_immediate(i32& cycles, Memory& memory) 
        -> std::expected<void, EmulatorError>;
    
    [[nodiscard]] constexpr auto execute_and_zero_page(i32& cycles, Memory& memory) 
        -> std::expected<void, EmulatorError>;
    
    [[nodiscard]] constexpr auto execute_and_zero_page_x(i32& cycles, Memory& memory) 
        -> std::expected<void, EmulatorError>;
    
    [[nodiscard]] constexpr auto execute_and_absolute(i32& cycles, Memory& memory) 
        -> std::expected<void, EmulatorError>;
    
    [[nodiscard]] constexpr auto execute_and_absolute_x(i32& cycles, Memory& memory)
	    -> std::expected<void, EmulatorError>;

    [[nodiscard]] constexpr auto execute_and_absolute_y(i32& cycles, Memory& memory)
	    -> std::expected<void, EmulatorError>;
    
    [[nodiscard]] constexpr auto execute_and_indirect_x(i32& cycles, Memory& memory)
	    -> std::expected<void, EmulatorError>;

    [[nodiscard]] constexpr auto execute_and_indirect_y(i32& cycles, Memory& memory)
	    -> std::expected<void, EmulatorError>;
};

inline constexpr void CPU::reset(Memory& memory) noexcept
{
    sp_ = INITIAL_SP;
    a_  = 0;
    x_  = 0;
    y_  = 0;
    flags_ = StatusFlags{};
    // memory.clear();

    // Read the start address FROM the reset vector
    auto reset_addr = memory.read_word(RESET_VECTOR);
    if (reset_addr) 
    {
        pc_ = reset_addr.value();
    } 
    else 
    {
        pc_ = 0x8000;  // Fallback
    }
}

inline constexpr auto CPU::fetch_byte(i32& cycles, Memory& memory)
    ->std::expected<u8, EmulatorError>
{
    auto result = memory.read_byte(pc_);
    if (!result) return result;
    pc_++;
    cycles--;
    return result;
}

inline constexpr auto CPU::fetch_word(i32& cycles, Memory& memory)
    -> std::expected<u16, EmulatorError>
{
    auto result = memory.read_word(pc_);
    if (!result) return result;
    pc_ += 2;
    cycles -= 2;
    return result;
}

inline constexpr auto CPU::read_byte(i32& cycles, u16 address, Memory& memory)
    -> std::expected<u8, EmulatorError>
{
    cycles--;
    return memory.read_byte(address);  // Fixed: was read_word
}

inline constexpr auto CPU::push_byte(i32& cycles, u8 value, Memory& memory)
    -> std::expected<void, EmulatorError>
{
    if (sp_ == 0) return std::unexpected(EmulatorError::StackUnderflow);
    auto result = memory.write_byte(STACK_PAGE + sp_, value);
    if (!result) return result;
    sp_--;
    cycles--;
    return {};
}

inline constexpr auto CPU::pop_byte(i32& cycles, Memory& memory)
    -> std::expected<u8, EmulatorError>
{
    if (sp_ == 0xFF) return std::unexpected(EmulatorError::StackOverflow);
    sp_++;
    cycles--;
    return memory.read_byte(STACK_PAGE + sp_);
}

inline constexpr void CPU::set_zn_flags(u8 value) noexcept
{
    flags_.zero     = (value == 0);
    flags_.negative = (value & 0b10000000) != 0;
}

inline constexpr void CPU::load_accumulator(u8 value) noexcept
{
    a_ = value;
    set_zn_flags(a_);
}

inline constexpr void CPU::load_x_register(u8 value) noexcept
{
    x_ = value;
    set_zn_flags(x_);
}

inline constexpr void CPU::load_y_register(u8 value) noexcept
{
    y_ = value;
    set_zn_flags(y_);
}

inline constexpr void CPU::add_with_carry(u8 value) noexcept
{
    const u16 carry = flags_.carry ? u16{1} : u16{0};
    const u16 sum = static_cast<u16>(static_cast<u16>(a_) + static_cast<u16>(value) + carry);
    
    // Check for signed overflow
    // Overflow occurs when:
    // - Adding two positive number gives a negative result
    // - Adding two negative numbers gives a positive result
    // Formula : ~(A ^ M) & (A ^ Result) & 0x80
    const bool overflow = (~(a_ ^ value) & (a_ ^ static_cast<u8>(sum)) & 0x80) != 0;

    a_ = static_cast<u8>(sum & 0xFF);

    flags_.carry    = (sum > 0xFF);
    flags_.zero     = (a_ == 0);
    flags_.negative = (a_ & 0x80) != 0;
    flags_.overflow = overflow;
}

inline constexpr void CPU::logical_and(u8 value) noexcept
{
    a_ &= value;
    set_zn_flags(a_);
}

// Instruction implementations
// Load Accumulator

inline constexpr auto CPU::execute_lda_immediate(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{
    auto value = fetch_byte(cycles, memory);
    if (!value) return std::unexpected(value.error());
    load_accumulator(value.value());
    return {};
}

inline constexpr auto CPU::execute_lda_zero_page(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{
    auto address = fetch_byte(cycles, memory);
    if (!address) return std::unexpected(address.error());

    auto value = read_byte(cycles, address.value(), memory);
    if (!value) return std::unexpected(value.error());

    load_accumulator(value.value());
    return {};
}

inline constexpr auto CPU::execute_lda_zero_page_x(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{
    auto address = fetch_byte(cycles, memory);
    if (!address) return std::unexpected(address.error());

    u8 final_address = address.value() + x_;
    cycles--;

    auto value = read_byte(cycles, final_address, memory);
    if (!value) return std::unexpected(value.error());

    load_accumulator(value.value());
    return {};
}

inline constexpr auto CPU::execute_lda_absolute(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{
    auto address = fetch_word(cycles, memory);
    if (!address) return std::unexpected(address.error());

    auto value = read_byte(cycles, address.value(), memory);
    if (!value) return std::unexpected(value.error());

    load_accumulator(value.value());
    return {};
}

inline constexpr auto CPU::execute_lda_absolute_x(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{
    auto address = fetch_word(cycles, memory);
    if (!address) return std::unexpected(address.error());

    u16 final_address = address.value() + x_;

    auto value = read_byte(cycles, final_address, memory);
    if (!value) return std::unexpected(value.error());

    if (page_crossed(address.value(), final_address))
    {
        cycles--;
    }

    load_accumulator(value.value());
    return {};
}

inline constexpr auto CPU::execute_lda_absolute_y(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{
    auto address = fetch_word(cycles, memory);
    if (!address) return std::unexpected(address.error());

    u16 final_address = address.value() + y_;

    auto value = read_byte(cycles, final_address, memory);
    if (!value) return std::unexpected(value.error());

    if (page_crossed(address.value(), final_address))
    {
        cycles--;
    }

    load_accumulator(value.value());
    return {};
}

// Control flow

inline constexpr auto CPU::execute_jsr(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{
    auto sub_address = fetch_word(cycles, memory);
    if (!sub_address) return std::unexpected(sub_address.error());

    const u16 return_address = pc_ - 1;

    cycles--; // Cycle 4: internal cycle

    auto push_high = push_byte(cycles, static_cast<u8>(return_address >> 8), memory);
    if (!push_high) return push_high;

    auto push_low = push_byte(cycles, static_cast<u8>(return_address & 0xFF), memory);
    if (!push_low) return push_low;

    pc_ = sub_address.value();

    return {};
}

inline constexpr auto CPU::execute_rts(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{
    cycles--; // Cycle 2: Internal cycle

    auto low = pop_byte(cycles, memory);
    if (!low) return std::unexpected(low.error());

    auto high = pop_byte(cycles, memory);
    if (!high) return std::unexpected(high.error());

    const u16 return_address = low.value() | (static_cast<u16>(high.value()) << 8);

    pc_ = return_address + 1;

    cycles -= 2; // Cycles 5-6: Increment PC and internal operations

    return {};
}

// Load X register

inline constexpr auto CPU::execute_ldx_immediate(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{
    auto value = fetch_byte(cycles, memory);
    if (!value) return std::unexpected(value.error());

    load_x_register(value.value());
    return {};
}

inline constexpr auto CPU::execute_ldx_zero_page(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{
    auto address = fetch_byte(cycles, memory);
    if (!address) return std::unexpected(address.error());

    auto value = read_byte(cycles, address.value(), memory);
    if (!value) return std::unexpected(address.error());

    load_x_register(value.value());
    return {};
}

inline constexpr auto CPU::execute_ldx_zero_page_y(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{
    auto initial_address = fetch_byte(cycles, memory);
    if (!initial_address) return std::unexpected(initial_address.error());

    u8 final_address = initial_address.value() + y_;
    cycles--;
    
    auto value = read_byte(cycles, final_address, memory);
    if (!value) return std::unexpected(value.error());
    
    load_x_register(value.value());
    return {};  
}

inline constexpr auto CPU::execute_ldx_absolute(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{
    auto address = fetch_word(cycles, memory);
    if (!address) return std::unexpected(address.error());

    auto value = read_byte(cycles, address.value(), memory);
    if (!value) return std::unexpected(value.error());

    load_x_register(value.value());
    return {};
}

inline constexpr auto CPU::execute_ldx_absolute_y(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{

    auto address = fetch_word(cycles, memory);
    if (!address) return std::unexpected(address.error());

    u16 final_address = address.value() + y_;

    auto value = read_byte(cycles, final_address, memory);
    if (!value) return std::unexpected(value.error());

    if (page_crossed(address.value(), final_address))
    {
        cycles--;
    }

    load_x_register(value.value());
    return {};
}

// Load Y register

inline constexpr auto CPU::execute_ldy_immediate(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{
    auto value = fetch_byte(cycles, memory);
    if (!value) return std::unexpected(value.error());

    load_y_register(value.value());
    return {};
}

inline constexpr auto CPU::execute_ldy_zero_page(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{
    auto address = fetch_byte(cycles, memory);
    if (!address) return std::unexpected(address.error());

    auto value = read_byte(cycles, address.value(), memory);
    if (!value) return std::unexpected(address.error());

    load_y_register(value.value());
    return {};
}

inline constexpr auto CPU::execute_ldy_zero_page_x(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{
    auto initial_address = fetch_byte(cycles, memory);
    if (!initial_address) return std::unexpected(initial_address.error());

    u8 final_address = initial_address.value() + x_;
    cycles--;
    
    auto value = read_byte(cycles, final_address, memory);
    if (!value) return std::unexpected(value.error());
    
    load_y_register(value.value());
    return {};  
}

inline constexpr auto CPU::execute_ldy_absolute(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{
    auto address = fetch_word(cycles, memory);
    if (!address) return std::unexpected(address.error());

    auto value = read_byte(cycles, address.value(), memory);
    if (!value) return std::unexpected(value.error());

    load_y_register(value.value());
    return {};
}

inline constexpr auto CPU::execute_ldy_absolute_x(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{

    auto address = fetch_word(cycles, memory);
    if (!address) return std::unexpected(address.error());

    u16 final_address = address.value() + x_;

    auto value = read_byte(cycles, final_address, memory);
    if (!value) return std::unexpected(value.error());

    if (page_crossed(address.value(), final_address))
    {
        cycles--;
    }

    load_y_register(value.value());
    return {};
}

// ADC Immediate Mode
inline constexpr auto CPU::execute_adc_immediate(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{
    auto value = fetch_byte(cycles, memory);
    if (!value) return std::unexpected(value.error());
    
    add_with_carry(value.value());
    return {};
}

// ADC Zero Page
inline constexpr auto CPU::execute_adc_zero_page(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{
    auto address = fetch_byte(cycles, memory);
    if (!address) return std::unexpected(address.error());
    
    auto value = read_byte(cycles, address.value(), memory);
    if (!value) return std::unexpected(value.error());
    
    add_with_carry(value.value());
    return {};
}

// ADC Zero Page, X
inline constexpr auto CPU::execute_adc_zero_page_x(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{
    auto address = fetch_byte(cycles, memory);
    if (!address) return std::unexpected(address.error());
    
    u8 final_address = address.value() + x_;
    cycles--;  // Extra cycle for index addition
    
    auto value = read_byte(cycles, final_address, memory);
    if (!value) return std::unexpected(value.error());
    
    add_with_carry(value.value());
    return {};
}

// ADC Absolute
inline constexpr auto CPU::execute_adc_absolute(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{
    auto address = fetch_word(cycles, memory);
    if (!address) return std::unexpected(address.error());
    
    auto value = read_byte(cycles, address.value(), memory);
    if (!value) return std::unexpected(value.error());
    
    add_with_carry(value.value());
    return {};
}

// ADC Absolute, X
inline constexpr auto CPU::execute_adc_absolute_x(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{
    auto address = fetch_word(cycles, memory);
    if (!address) return std::unexpected(address.error());
    
    u16 final_address = address.value() + x_;
    
    auto value = read_byte(cycles, final_address, memory);
    if (!value) return std::unexpected(value.error());
    
    if (page_crossed(address.value(), final_address))
    {
        cycles--;  // Extra cycle for page boundary crossing
    }
    
    add_with_carry(value.value());
    return {};
}

// ADC Absolute, Y
inline constexpr auto CPU::execute_adc_absolute_y(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{
    auto address = fetch_word(cycles, memory);
    if (!address) return std::unexpected(address.error());
    
    u16 final_address = address.value() + y_;
    
    auto value = read_byte(cycles, final_address, memory);
    if (!value) return std::unexpected(value.error());
    
    if (page_crossed(address.value(), final_address))
    {
        cycles--;  // Extra cycle for page boundary crossing
    }
    
    add_with_carry(value.value());
    return {};
}

// ADC Indirect, X
inline constexpr auto CPU::execute_adc_indirect_x(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{
    auto zero_page_addr = fetch_byte(cycles, memory);
    if (!zero_page_addr) return std::unexpected(zero_page_addr.error());
    
    u8 indexed_addr = zero_page_addr.value() + x_;
    cycles--;  // Extra cycle for index addition
    
    auto effective_addr = memory.read_word(indexed_addr);
    if (!effective_addr) return std::unexpected(effective_addr.error());
    cycles -= 2;  // Two cycles to read word from zero page
    
    auto value = read_byte(cycles, effective_addr.value(), memory);
    if (!value) return std::unexpected(value.error());
    
    add_with_carry(value.value());
    return {};
}

// ADC Indirect, Y
inline constexpr auto CPU::execute_adc_indirect_y(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{
    auto zero_page_addr = fetch_byte(cycles, memory);
    if (!zero_page_addr) return std::unexpected(zero_page_addr.error());
    
    auto base_addr = memory.read_word(zero_page_addr.value());
    if (!base_addr) return std::unexpected(base_addr.error());
    cycles -= 2;  // Two cycles to read word from zero page
    
    u16 final_address = base_addr.value() + y_;
    
    auto value = read_byte(cycles, final_address, memory);
    if (!value) return std::unexpected(value.error());
    
    if (page_crossed(base_addr.value(), final_address))
    {
        cycles--;  // Extra cycle for page boundary crossing
    }
    
    add_with_carry(value.value());
    return {};
}

// AND Immediate Mode
inline constexpr auto CPU::execute_and_immediate(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{
    auto value = fetch_byte(cycles, memory);
    if (!value) return std::unexpected(value.error());
    
    logical_and(value.value());
    return {};
}

// AND Zero Page
inline constexpr auto CPU::execute_and_zero_page(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{
    auto address = fetch_byte(cycles, memory);
    if (!address) return std::unexpected(address.error());
    
    auto value = read_byte(cycles, address.value(), memory);
    if (!value) return std::unexpected(value.error());
    
    logical_and(value.value());
    return {};
}

// AND Zero Page, X
inline constexpr auto CPU::execute_and_zero_page_x(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{
    auto address = fetch_byte(cycles, memory);
    if (!address) return std::unexpected(address.error());
    
    u8 final_address = address.value() + x_;
    cycles--;  // Extra cycle for index addition
    
    auto value = read_byte(cycles, final_address, memory);
    if (!value) return std::unexpected(value.error());
    
    logical_and(value.value());
    return {};
}

// AND Absolute
inline constexpr auto CPU::execute_and_absolute(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{
    auto address = fetch_word(cycles, memory);
    if (!address) return std::unexpected(address.error());
    
    auto value = read_byte(cycles, address.value(), memory);
    if (!value) return std::unexpected(value.error());
    
    logical_and(value.value());
    return {};
}

// AND Absolute, X
inline constexpr auto CPU::execute_and_absolute_x(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{
    auto address = fetch_word(cycles, memory);
    if (!address) return std::unexpected(address.error());
    
    u16 final_address = address.value() + x_;
    
    auto value = read_byte(cycles, final_address, memory);
    if (!value) return std::unexpected(value.error());
    
    if (page_crossed(address.value(), final_address))
    {
        cycles--;  // Extra cycle for page boundary crossing
    }
    
    logical_and(value.value());
    return {};
}

// AND Absolute, Y
inline constexpr auto CPU::execute_and_absolute_y(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{
    auto address = fetch_word(cycles, memory);
    if (!address) return std::unexpected(address.error());
    
    u16 final_address = address.value() + y_;
    
    auto value = read_byte(cycles, final_address, memory);
    if (!value) return std::unexpected(value.error());
    
    if (page_crossed(address.value(), final_address))
    {
        cycles--;  // Extra cycle for page boundary crossing
    }
    
    logical_and(value.value());
    return {};
}

// AND Indirect, X
inline constexpr auto CPU::execute_and_indirect_x(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{
    auto zero_page_addr = fetch_byte(cycles, memory);
    if (!zero_page_addr) return std::unexpected(zero_page_addr.error());
    
    u8 indexed_addr = zero_page_addr.value() + x_;
    cycles--;  // Extra cycle for index addition
    
    auto effective_addr = memory.read_word(indexed_addr);
    if (!effective_addr) return std::unexpected(effective_addr.error());
    cycles -= 2;  // Two cycles to read word from zero page
    
    auto value = read_byte(cycles, effective_addr.value(), memory);
    if (!value) return std::unexpected(value.error());
    
    logical_and(value.value());
    return {};
}

// AND Indirect, Y
inline constexpr auto CPU::execute_and_indirect_y(i32& cycles, Memory& memory)
    -> std::expected<void, EmulatorError>
{
    auto zero_page_addr = fetch_byte(cycles, memory);
    if (!zero_page_addr) return std::unexpected(zero_page_addr.error());
    
    auto base_addr = memory.read_word(zero_page_addr.value());
    if (!base_addr) return std::unexpected(base_addr.error());
    cycles -= 2;  // Two cycles to read word from zero page
    
    u16 final_address = base_addr.value() + y_;
    
    auto value = read_byte(cycles, final_address, memory);
    if (!value) return std::unexpected(value.error());
    
    if (page_crossed(base_addr.value(), final_address))
    {
        cycles--;  // Extra cycle for page boundary crossing
    }
    
    logical_and(value.value());
    return {};
}
} // namespace cpu6502
