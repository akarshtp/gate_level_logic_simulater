/**
 * @file AndGate.h
 * @brief AND gate primitive with configurable input count.
 *
 * Output is HIGH only when ALL inputs are HIGH.
 * Supports 3-valued logic: if any input is LOW, output is LOW;
 * if any input is UNKNOWN (and none are LOW), output is UNKNOWN.
 */

#pragma once

#include "Gate.h"

/**
 * @class AndGate
 * @brief Multi-input AND gate with propagation delay.
 */
class AndGate : public Gate {
public:
    /**
     * @brief Construct an AND gate.
     * @param name              Gate identifier.
     * @param propagation_delay Delay in simulation time units (default: 1).
     */
    AndGate(const std::string& name, uint64_t propagation_delay = 1);

    /**
     * @brief Evaluate AND logic on all input nets.
     * @return HIGH if all inputs are HIGH, LOW if any input is LOW,
     *         UNKNOWN otherwise.
     */
    Signal evaluate() override;
};
