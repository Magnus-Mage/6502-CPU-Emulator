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

// Inline implementations
inline constexpr Memory::Memory() : data_{} {}

inline constexpr auto Memory::read_byte(u16 address) const
    -> std::expected<u8, EmulatorError>
{
    if (address >= MAX_MEM) {
        return std::unexpected(EmulatorError::InvalidAddress);
    }
    return data_[address];
}

inline constexpr auto Memory::read_word(u16 address) const
    -> std::expected<u16, EmulatorError>
{
    if (static_cast<u32>(address) + 1u >= MAX_MEM) {
        return std::unexpected(EmulatorError::InvalidAddress);
    }
    u16 low  = data_[address];
    u16 high = data_[address + 1];
    return low | (high << 8);
}

inline constexpr auto Memory::write_byte(u16 address, u8 value)
    -> std::expected<void, EmulatorError>
{
    if (address >= MAX_MEM) {
        return std::unexpected(EmulatorError::InvalidAddress);
    }
    data_[address] = value;
    return {};
}

inline constexpr auto Memory::write_word(u16 address, u16 value)
    -> std::expected<void, EmulatorError>
{
    if (static_cast<u32>(address) + 1u >= MAX_MEM) {
        return std::unexpected(EmulatorError::InvalidAddress);
    }
    data_[address]     = static_cast<u8>(value & 0xFF);
    data_[address + 1] = static_cast<u8>(value >> 8);
    return {};
}

inline constexpr void Memory::clear() noexcept {
    data_.fill(0);
}

inline constexpr u8& Memory::operator[](u16 address) noexcept {
    return data_[address];
}

inline constexpr const u8& Memory::operator[](u16 address) const noexcept {
    return data_[address];
}

} // namespace cpu6502
