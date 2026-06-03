/**
 * @file Gate.cpp
 * @brief Implementation of the Gate abstract base class.
 */

#include "Gate.h"
#include "Net.h"

Gate::Gate(const std::string& name, uint64_t propagation_delay)
    : name_(name)
    , propagation_delay_(propagation_delay)
    , output_(nullptr)
{
}

const std::string& Gate::getName() const {
    return name_;
}

uint64_t Gate::getPropagationDelay() const {
    return propagation_delay_;
}

void Gate::setPropagationDelay(uint64_t delay) {
    propagation_delay_ = delay;
}

void Gate::addInput(Net* net) {
    inputs_.push_back(net);
}

void Gate::setOutput(Net* net) {
    output_ = net;
}

const std::vector<Net*>& Gate::getInputs() const {
    return inputs_;
}

Net* Gate::getOutput() const {
    return output_;
}
