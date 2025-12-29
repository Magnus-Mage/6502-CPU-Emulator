#pragma once

namespace cpu6502 {

/**
 * @type Enum class
 * @brief Enum class for better error handling
 */
enum class EmulatorError {
    InvalidAddress,
    StackOverflow,
    InvalidOpcode,
    StackUnderflow,
    InsufficientCycles
};

/**
 * @brief Helper func to get error message
 */
constexpr const char* error_message(EmulatorError error) noexcept {
    switch (error) {
        case EmulatorError::InvalidAddress:
            return "Invalid memory address";
        case EmulatorError::StackOverflow:
            return "Stack Overflow";
        case EmulatorError::InvalidOpcode:
            return "Invalid Opcode used";
        case EmulatorError::StackUnderflow:
            return "Stack Underflow";
        case EmulatorError::InsufficientCycles:
            return "InSufficient Cycles used";
        default:
            return "Unknown Error: Check source";
    }
}

}  // namespace cpu6502
