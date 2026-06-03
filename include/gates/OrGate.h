/**
 * @file OrGate.h
 * @brief OR gate primitive with configurable input count.
 *
 * Output is HIGH when ANY input is HIGH.
 * Supports 3-valued logic: if any input is HIGH, output is HIGH;
 * if any input is UNKNOWN (and none are HIGH), output is UNKNOWN.
 */

#pragma once

#include "Gate.h"

/**
 * @class OrGate
 * @brief Multi-input OR gate with propagation delay.
 */
class OrGate : public Gate {
public:
    /**
     * @brief Construct an OR gate.
     * @param name              Gate identifier.
     * @param propagation_delay Delay in simulation time units (default: 1).
     */
    OrGate(const std::string& name, uint64_t propagation_delay = 1);

    /**
     * @brief Evaluate OR logic on all input nets.
     * @return HIGH if any input is HIGH, LOW if all inputs are LOW,
     *         UNKNOWN otherwise.
     */
    Signal evaluate() override;
};
