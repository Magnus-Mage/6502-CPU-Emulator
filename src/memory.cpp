#include "cpu6502/memory.hpp"

namespace cpu6502
{
constexpr Memory::Memory() : data_{} {}

constexpr auto Memory::read_byte(u16 address) const
    -> std::expected<u8, EmulatorError>
{
    if (address >= MAX_MEM) {
        return std::unexpected(EmulatorError::InvalidAddress);
    }
    return data_[address];
}

constexpr auto Memory::read_word(u16 address) const
    -> std::expected<u16, EmulatorError>
{
    if (static_cast<u32>(address) + 1u >= MAX_MEM) {
        return std::unexpected(EmulatorError::InvalidAddress);
    }
    u16 low  = data_[address];
    u16 high = data_[address + 1];
    return low | (high << 8);
}

constexpr auto Memory::write_byte(u16 address, u8 value)
    -> std::expected<void, EmulatorError>
{
    if (address >= MAX_MEM) {
        return std::unexpected(EmulatorError::InvalidAddress);
    }
    data_[address] = value;
    return {};
}

constexpr auto Memory::write_word(u16 address, u16 value)
    -> std::expected<void, EmulatorError>
{
    if (static_cast<u32>(address) + 1u >= MAX_MEM) {
        return std::unexpected(EmulatorError::InvalidAddress);
    }
    data_[address]     = static_cast<u8>(value & 0xFF);
    data_[address + 1] = static_cast<u8>(value >> 8);
    return {};
}

constexpr void Memory::clear() noexcept {
    data_.fill(0);
}

constexpr u8& Memory::operator[](u16 address) noexcept {
    return data_[address];
}

constexpr const u8& Memory::operator[](u16 address) const noexcept {
    return data_[address];
}

} // namespace cpu6502
