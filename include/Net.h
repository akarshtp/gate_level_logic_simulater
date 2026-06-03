/**
 * @file Net.h
 * @brief Represents a wire/signal net in the digital circuit.
 *
 * A Net carries a Signal value and maintains a fanout list of gates
 * that are driven by (have this net as an input to) them. When the
 * net's value changes, the simulation engine uses the fanout list
 * to determine which gates need re-evaluation.
 */

#pragma once

#include "Signal.h"
#include <string>
#include <vector>

class Gate;  // Forward declaration

/**
 * @class Net
 * @brief A signal-carrying wire in the simulated circuit.
 */
class Net {
public:
    /**
     * @brief Construct a Net with a given name and initial signal value.
     * @param name   Unique identifier for this net (e.g., "A", "clk", "wire_3").
     * @param initial Initial signal value (defaults to UNKNOWN / X).
     */
    explicit Net(const std::string& name, Signal initial = Signal::UNKNOWN);

    // ── Accessors ─────────────────────────────────────

    /** @brief Get the net's name. */
    const std::string& getName() const;

    /** @brief Get the current signal value on this net. */
    Signal getValue() const;

    /**
     * @brief Set the signal value on this net.
     * @param value New signal value.
     */
    void setValue(Signal value);

    // ── Fanout management ─────────────────────────────

    /**
     * @brief Register a gate as a fanout (consumer) of this net.
     * @param gate Pointer to the gate that reads this net as an input.
     */
    void addFanoutGate(Gate* gate);

    /**
     * @brief Get all gates in the fanout list.
     * @return Const reference to the vector of fanout gate pointers.
     */
    const std::vector<Gate*>& getFanoutGates() const;

private:
    std::string          name_;
    Signal               value_;
    std::vector<Gate*>   fanout_gates_;
};
