/**
 * @file SimulationEngine.cpp
 * @brief Implementation of the event-driven simulation engine.
 *
 * Core simulation loop:
 *   1. Pop earliest event from priority_queue (min-heap)
 *   2. Update target net's signal value
 *   3. Lookup fanout gates via unordered_map
 *   4. Evaluate each affected gate (virtual dispatch)
 *   5. If output changed, schedule new event with propagation delay
 */

#include "SimulationEngine.h"
#include "Net.h"
#include "Gate.h"

#include <iostream>

// ─────────────────────────────────────────────────────
// Construction / Destruction
// ─────────────────────────────────────────────────────

SimulationEngine::SimulationEngine()
    : current_time_(0)
    , events_processed_(0)
{
}

SimulationEngine::~SimulationEngine() = default;

// ─────────────────────────────────────────────────────
// Circuit construction
// ─────────────────────────────────────────────────────

void SimulationEngine::addNet(Net* net) {
    nets_.push_back(net);
}

void SimulationEngine::addGate(Gate* gate) {
    gates_.push_back(gate);
}

void SimulationEngine::buildFanoutMap() {
    fanout_map_.clear();

    // For each gate, register it in the fanout list of every input net
    for (Gate* gate : gates_) {
        for (Net* input_net : gate->getInputs()) {
            fanout_map_[input_net].push_back(gate);
        }
    }
}

// ─────────────────────────────────────────────────────
// Event scheduling
// ─────────────────────────────────────────────────────

void SimulationEngine::scheduleEvent(uint64_t time, Net* net, Signal value) {
    event_queue_.push({time, net, value});
}

// ─────────────────────────────────────────────────────
// Simulation execution
// ─────────────────────────────────────────────────────

void SimulationEngine::run(uint64_t max_time) {
    while (!event_queue_.empty()) {
        // Peek at the earliest event
        Event evt = event_queue_.top();
        event_queue_.pop();

        // Stop if we've exceeded the time limit
        if (evt.time > max_time) {
            break;
        }

        // Advance simulation time
        current_time_ = evt.time;
        ++events_processed_;

        // Skip if no actual value change on the net
        if (evt.target_net->getValue() == evt.new_value) {
            continue;
        }

        // Update the net's value
        evt.target_net->setValue(evt.new_value);

        // Propagate: find all gates driven by this net via fanout map
        auto it = fanout_map_.find(evt.target_net);
        if (it == fanout_map_.end()) {
            continue;  // No gates to propagate to
        }

        // Evaluate each gate in the fanout
        for (Gate* gate : it->second) {
            Net* output_net = gate->getOutput();
            if (!output_net) {
                continue;  // Gate has no output net connected
            }

            // Save current output value
            Signal old_output = output_net->getValue();

            // Compute new output via virtual dispatch
            Signal new_output = gate->evaluate();

            // If output changed, schedule event at future time
            if (new_output != old_output) {
                uint64_t event_time = current_time_ + gate->getPropagationDelay();
                scheduleEvent(event_time, output_net, new_output);
            }
        }
    }
}

void SimulationEngine::reset() {
    // Clear the event queue
    while (!event_queue_.empty()) {
        event_queue_.pop();
    }

    fanout_map_.clear();
    current_time_ = 0;
    events_processed_ = 0;
}

// ─────────────────────────────────────────────────────
// Accessors
// ─────────────────────────────────────────────────────

uint64_t SimulationEngine::getCurrentTime() const {
    return current_time_;
}

uint64_t SimulationEngine::getEventsProcessed() const {
    return events_processed_;
}

const std::vector<Net*>& SimulationEngine::getNets() const {
    return nets_;
}

const std::vector<Gate*>& SimulationEngine::getGates() const {
    return gates_;
}
