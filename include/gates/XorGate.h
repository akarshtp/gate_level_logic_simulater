/**
 * @file XorGate.h
 * @brief XOR (exclusive-or) gate primitive.
 *
 * Two-input gate: output is HIGH when inputs differ.
 * Supports multi-input cascaded XOR (odd-parity detector).
 * UNKNOWN on any input produces UNKNOWN output.
 */

#pragma once

#include "Gate.h"

/**
 * @class XorGate
 * @brief Multi-input XOR gate with propagation delay.
 */
class XorGate : public Gate {
public:
    /**
     * @brief Construct an XOR gate.
     * @param name              Gate identifier.
     * @param propagation_delay Delay in simulation time units (default: 1).
     */
    XorGate(const std::string& name, uint64_t propagation_delay = 1);

    /**
     * @brief Evaluate XOR logic on all input nets.
     * @return HIGH if an odd number of inputs are HIGH, LOW if even,
     *         UNKNOWN if any input is UNKNOWN.
     */
    Signal evaluate() override;
};
