/**
 * @file XorGate.cpp
 * @brief Implementation of the XOR gate with 3-valued logic.
 *
 * Multi-input XOR behaves as an odd-parity detector:
 * output is HIGH when an odd number of inputs are HIGH.
 */

#include "gates/XorGate.h"
#include "Net.h"

XorGate::XorGate(const std::string& name, uint64_t propagation_delay)
    : Gate(name, propagation_delay)
{
}

Signal XorGate::evaluate() {
    if (inputs_.empty()) {
        return Signal::UNKNOWN;
    }

    bool parity = false;  // Tracks odd/even count of HIGH inputs

    for (const Net* net : inputs_) {
        Signal val = net->getValue();
        if (val == Signal::UNKNOWN) {
            return Signal::UNKNOWN;  // Any unknown makes XOR indeterminate
        }
        if (val == Signal::HIGH) {
            parity = !parity;
        }
    }

    return parity ? Signal::HIGH : Signal::LOW;
}
