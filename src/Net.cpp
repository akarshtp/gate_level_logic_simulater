/**
 * @file Net.cpp
 * @brief Implementation of the Net class.
 */

#include "Net.h"
#include "Gate.h"

Net::Net(const std::string& name, Signal initial)
    : name_(name)
    , value_(initial)
{
}

const std::string& Net::getName() const {
    return name_;
}

Signal Net::getValue() const {
    return value_;
}

void Net::setValue(Signal value) {
    value_ = value;
}

void Net::addFanoutGate(Gate* gate) {
    fanout_gates_.push_back(gate);
}

const std::vector<Gate*>& Net::getFanoutGates() const {
    return fanout_gates_;
}
