/**
 * @file OrGate.cpp
 * @brief Implementation of the OR gate with 3-valued logic.
 */

#include "gates/OrGate.h"
#include "Net.h"

OrGate::OrGate(const std::string& name, uint64_t propagation_delay)
    : Gate(name, propagation_delay)
{
}

Signal OrGate::evaluate() {
    if (inputs_.empty()) {
        return Signal::UNKNOWN;
    }

    bool has_unknown = false;

    for (const Net* net : inputs_) {
        Signal val = net->getValue();
        if (val == Signal::HIGH) {
            return Signal::HIGH;  // Short-circuit: any HIGH makes OR HIGH
        }
        if (val == Signal::UNKNOWN) {
            has_unknown = true;
        }
    }

    // If we get here, no inputs are HIGH
    return has_unknown ? Signal::UNKNOWN : Signal::LOW;
}
