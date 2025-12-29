#pragma once

#include "types.hpp"

namespace cpu6502 {

/**
 * @type struct
 * @brief status flags with field names
 */
struct StatusFlags {
    bool carry : 1     = false;
    bool zero : 1      = false;
    bool interrupt : 1 = false;
    bool decimal : 1   = false;
    bool brk : 1       = false;
    bool overflow : 1  = false;
    bool negative : 1  = false;

    constexpr StatusFlags() = default;

    // Convert to byte representation
    [[nodiscard]] constexpr u8 to_byte() const noexcept {
        return (carry << 0) | (zero << 1) | (interrupt << 2) | (decimal << 3) | (brk << 4) |
               (1 << 5) |  // Unused bit always set
               (overflow << 6) | (negative << 7);
    }

    // Convert from byte
    [[nodiscard]] constexpr StatusFlags from_byte(u8 value) noexcept {
        StatusFlags flags;

        flags.carry     = (value & 0x01) != 0;
        flags.zero      = (value & 0x02) != 0;
        flags.interrupt = (value & 0x04) != 0;
        flags.decimal   = (value & 0x08) != 0;
        flags.brk       = (value & 0x10) != 0;
        flags.overflow  = (value & 0x40) != 0;
        flags.negative  = (value & 0x80) != 0;
        return flags;
    }
};

}  // namespace cpu6502
