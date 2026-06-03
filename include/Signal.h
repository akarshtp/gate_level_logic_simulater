/**
 * @file Signal.h
 * @brief Three-valued logic signal representation for digital simulation.
 *
 * Defines the fundamental signal states used throughout the simulator:
 * LOW (logic 0), HIGH (logic 1), and UNKNOWN (uninitialized / X).
 */

#pragma once

#include <cstdint>

/**
 * @enum Signal
 * @brief Represents a three-valued digital logic signal.
 */
enum class Signal : uint8_t {
    LOW     = 0,  ///< Logic 0
    HIGH    = 1,  ///< Logic 1
    UNKNOWN = 2   ///< Uninitialized or indeterminate (X)
};

/**
 * @brief Convert a Signal to its single-character representation.
 * @param s The signal value.
 * @return '0' for LOW, '1' for HIGH, 'X' for UNKNOWN.
 */
inline char signalToChar(Signal s) {
    switch (s) {
        case Signal::LOW:     return '0';
        case Signal::HIGH:    return '1';
        case Signal::UNKNOWN: return 'X';
        default:              return '?';
    }
}

/**
 * @brief Convert a Signal to a human-readable string.
 * @param s The signal value.
 * @return "LOW", "HIGH", or "UNKNOWN".
 */
inline const char* signalToString(Signal s) {
    switch (s) {
        case Signal::LOW:     return "LOW";
        case Signal::HIGH:    return "HIGH";
        case Signal::UNKNOWN: return "UNKNOWN";
        default:              return "???";
    }
}

/**
 * @brief Convert a boolean to a Signal.
 * @param b Boolean value.
 * @return Signal::HIGH if true, Signal::LOW if false.
 */
inline Signal boolToSignal(bool b) {
    return b ? Signal::HIGH : Signal::LOW;
}

/**
 * @brief Convert a Signal to boolean (UNKNOWN treated as false).
 * @param s The signal value.
 * @return true if HIGH, false otherwise.
 */
inline bool signalToBool(Signal s) {
    return s == Signal::HIGH;
}
