/**
 * @file AndGate.cpp
 * @brief Implementation of the AND gate with 3-valued logic.
 */

#include "gates/AndGate.h"
#include "Net.h"

AndGate::AndGate(const std::string& name, uint64_t propagation_delay)
    : Gate(name, propagation_delay)
{
}

Signal AndGate::evaluate() {
    if (inputs_.empty()) {
        return Signal::UNKNOWN;
    }

    bool has_unknown = false;

    for (const Net* net : inputs_) {
        Signal val = net->getValue();
        if (val == Signal::LOW) {
            return Signal::LOW;  // Short-circuit: any LOW makes AND LOW
        }
        if (val == Signal::UNKNOWN) {
            has_unknown = true;
        }
    }

    // If we get here, no inputs are LOW
    return has_unknown ? Signal::UNKNOWN : Signal::HIGH;
}
