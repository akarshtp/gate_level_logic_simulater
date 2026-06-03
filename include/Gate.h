/**
 * @file Gate.h
 * @brief Abstract base class for all logic gate primitives.
 *
 * Gate defines the interface for all combinational and sequential
 * logic elements in the simulator. Concrete gate types (AND, OR, etc.)
 * inherit from Gate and implement the pure virtual evaluate() method.
 *
 * Design: Uses inheritance hierarchy with virtual dispatch, as is
 * standard in event-driven gate-level simulators.
 */

#pragma once

#include "Signal.h"
#include <vector>
#include <string>
#include <cstdint>

class Net;  // Forward declaration

/**
 * @class Gate
 * @brief Abstract base class for logic gate primitives.
 *
 * Each gate has:
 * - A set of input nets
 * - An output net
 * - A configurable propagation delay
 * - A virtual evaluate() method that computes the output signal
 *   based on current input values
 */
class Gate {
public:
    /**
     * @brief Construct a gate with a name and propagation delay.
     * @param name              Human-readable identifier (e.g., "U1_AND").
     * @param propagation_delay Gate delay in simulation time units (default: 1).
     */
    Gate(const std::string& name, uint64_t propagation_delay = 1);

    /** @brief Virtual destructor for proper cleanup of derived classes. */
    virtual ~Gate() = default;

    // ── Core interface ────────────────────────────────

    /**
     * @brief Evaluate the gate's output based on current input net values.
     *
     * This pure virtual method must be implemented by each concrete gate type.
     * It reads the current values from all input nets and computes what the
     * output signal should be. The simulation engine uses this to determine
     * if a new event should be scheduled on the output net.
     *
     * @return The computed output Signal value.
     */
    virtual Signal evaluate() = 0;

    // ── Accessors ─────────────────────────────────────

    /** @brief Get the gate's name. */
    const std::string& getName() const;

    /** @brief Get the propagation delay in time units. */
    uint64_t getPropagationDelay() const;

    /**
     * @brief Set the propagation delay.
     * @param delay New delay in simulation time units.
     */
    void setPropagationDelay(uint64_t delay);

    // ── Net connectivity ──────────────────────────────

    /**
     * @brief Add an input net to this gate.
     * @param net Pointer to the input net.
     */
    void addInput(Net* net);

    /**
     * @brief Set the output net of this gate.
     * @param net Pointer to the output net.
     */
    void setOutput(Net* net);

    /** @brief Get the list of input nets. */
    const std::vector<Net*>& getInputs() const;

    /** @brief Get the output net. */
    Net* getOutput() const;

protected:
    std::string        name_;
    uint64_t           propagation_delay_;
    std::vector<Net*>  inputs_;
    Net*               output_;
};
