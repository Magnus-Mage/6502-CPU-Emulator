#pragma once

#include <concepts>
#include <cstdint>

namespace cpu6502 {

/**
 * Modern type alias using standard fixed-width types
 */
using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

/**
 * @brief concept for the address types
 */
template <typename T>
concept AddressType = std::unsigned_integral<T> && sizeof(T) <= sizeof(u16);

}  // namespace cpu6502
