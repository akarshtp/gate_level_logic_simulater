/**
 * @file NotGate.h
 * @brief NOT (inverter) gate primitive.
 *
 * Single-input gate that inverts the signal.
 * UNKNOWN input produces UNKNOWN output.
 */

#pragma once

#include "Gate.h"

/**
 * @class NotGate
 * @brief Single-input inverter gate with propagation delay.
 */
class NotGate : public Gate {
public:
    /**
     * @brief Construct a NOT gate.
     * @param name              Gate identifier.
     * @param propagation_delay Delay in simulation time units (default: 1).
     */
    NotGate(const std::string& name, uint64_t propagation_delay = 1);

    /**
     * @brief Evaluate NOT logic on the single input net.
     * @return HIGH if input is LOW, LOW if input is HIGH, UNKNOWN otherwise.
     */
    Signal evaluate() override;
};
