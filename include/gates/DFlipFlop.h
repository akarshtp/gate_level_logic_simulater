/**
 * @file DFlipFlop.h
 * @brief Rising-edge triggered D Flip-Flop primitive.
 *
 * Inputs in order:
 *   - inputs_[0]: D (Data input)
 *   - inputs_[1]: CLK (Clock input)
 *
 * Output:
 *   - output_: Q (Lapped state output)
 */

#pragma once

#include "Gate.h"

/**
 * @class DFlipFlop
 * @brief Rising-edge triggered D Flip-Flop with propagation delay.
 */
class DFlipFlop : public Gate {
public:
    /**
     * @brief Construct a D Flip-Flop.
     * @param name              Gate identifier.
     * @param propagation_delay Delay in simulation time units (default: 1).
     */
    DFlipFlop(const std::string& name, uint64_t propagation_delay = 1);

    /**
     * @brief Evaluate D Flip-Flop state.
     *
     * Detects a rising edge on the clock input (inputs_[1]) and latches
     * the data input (inputs_[0]).
     *
     * @return The stored state value (Q).
     */
    Signal evaluate() override;

    /**
     * @brief Access the internal stored state (for testing/monitoring).
     */
    Signal getStoredState() const;

private:
    Signal prev_clk_; ///< Historical CLK value to detect rising edge
    Signal stored_q_; ///< Internally stored state
};
