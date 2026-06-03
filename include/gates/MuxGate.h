/**
 * @file MuxGate.h
 * @brief 2:1 Multiplexer gate primitive.
 *
 * Inputs in order:
 *   - inputs_[0]: D0 (input selected when SEL is LOW)
 *   - inputs_[1]: D1 (input selected when SEL is HIGH)
 *   - inputs_[2]: SEL (selector line)
 */

#pragma once

#include "Gate.h"

/**
 * @class MuxGate
 * @brief 2:1 multiplexer primitive with propagation delay.
 */
class MuxGate : public Gate {
public:
    /**
     * @brief Construct a MUX gate.
     * @param name              Gate identifier.
     * @param propagation_delay Delay in simulation time units (default: 1).
     */
    MuxGate(const std::string& name, uint64_t propagation_delay = 1);

    /**
     * @brief Evaluate MUX logic.
     * @return D0 value if SEL is LOW, D1 value if SEL is HIGH,
     *         common value if SEL is UNKNOWN but D0 == D1,
     *         UNKNOWN otherwise.
     */
    Signal evaluate() override;
};
