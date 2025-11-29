#pragma once

#include "types.hpp"
#include "error.hpp"
#include <array>
#include <expected>

namespace cpu6502
{

/**
 * @type Memory class
 * @brief Memory class that will define our memory subsystem
 */
class Memory {
public:
    static constexpr u32 MAX_MEM = 1024 * 64;
    
    constexpr Memory();
    
    // Read operations
    [[nodiscard]] constexpr auto read_byte(u16 address) const 
        -> std::expected<u8, EmulatorError>;
    
    [[nodiscard]] constexpr auto read_word(u16 address) const 
        -> std::expected<u16, EmulatorError>;
    
    // Write operations
    constexpr auto write_byte(u16 address, u8 value) 
        -> std::expected<void, EmulatorError>;
    
    constexpr auto write_word(u16 address, u16 value) 
        -> std::expected<void, EmulatorError>;
    
    // Utility
    constexpr void clear() noexcept;
    
    // Direct access for setup (use carefully)
    constexpr u8& operator[](u16 address) noexcept;
    constexpr const u8& operator[](u16 address) const noexcept;
    
private:
    std::array<u8, MAX_MEM> data_;
};


} // namespace cpu6502
