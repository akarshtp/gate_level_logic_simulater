/**
 * @file NotGate.cpp
 * @brief Implementation of the NOT (inverter) gate.
 */

#include "gates/NotGate.h"
#include "Net.h"

NotGate::NotGate(const std::string& name, uint64_t propagation_delay)
    : Gate(name, propagation_delay)
{
}

Signal NotGate::evaluate() {
    if (inputs_.empty()) {
        return Signal::UNKNOWN;
    }

    Signal input = inputs_[0]->getValue();

    switch (input) {
        case Signal::LOW:     return Signal::HIGH;
        case Signal::HIGH:    return Signal::LOW;
        case Signal::UNKNOWN: return Signal::UNKNOWN;
        default:              return Signal::UNKNOWN;
    }
}
