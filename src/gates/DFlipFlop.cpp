/**
 * @file DFlipFlop.cpp
 * @brief Implementation of the rising-edge triggered D Flip-Flop.
 */

#include "gates/DFlipFlop.h"
#include "Net.h"

DFlipFlop::DFlipFlop(const std::string& name, uint64_t propagation_delay)
    : Gate(name, propagation_delay)
    , prev_clk_(Signal::UNKNOWN)
    , stored_q_(Signal::UNKNOWN)
{
}

Signal DFlipFlop::evaluate() {
    if (inputs_.size() < 2) {
        return Signal::UNKNOWN;
    }

    Signal d   = inputs_[0]->getValue();
    Signal clk = inputs_[1]->getValue();

    // Rising edge detection: LOW -> HIGH transition on CLK line
    if (prev_clk_ == Signal::LOW && clk == Signal::HIGH) {
        stored_q_ = d;
    }

    prev_clk_ = clk;
    return stored_q_;
}

Signal DFlipFlop::getStoredState() const {
    return stored_q_;
}
