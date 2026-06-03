/**
 * @file MuxGate.cpp
 * @brief Implementation of the 2:1 Multiplexer gate.
 */

#include "gates/MuxGate.h"
#include "Net.h"

MuxGate::MuxGate(const std::string& name, uint64_t propagation_delay)
    : Gate(name, propagation_delay)
{
}

Signal MuxGate::evaluate() {
    if (inputs_.size() < 3) {
        return Signal::UNKNOWN;
    }

    Signal d0  = inputs_[0]->getValue();
    Signal d1  = inputs_[1]->getValue();
    Signal sel = inputs_[2]->getValue();

    if (sel == Signal::LOW) {
        return d0;
    } else if (sel == Signal::HIGH) {
        return d1;
    } else {
        // SEL is UNKNOWN. If D0 and D1 are the same, the output is known!
        if (d0 == d1) {
            return d0;
        }
        return Signal::UNKNOWN;
    }
}
