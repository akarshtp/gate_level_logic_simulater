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
#include <unordered_set>

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

void SimulationEngine::generateClock(Net* net, uint64_t period, uint64_t cycles, uint64_t start_time) {
    uint64_t half_period = period / 2;
    for (uint64_t i = 0; i < cycles; ++i) {
        uint64_t cycle_start = start_time + i * period;
        // Rising edge
        scheduleEvent(cycle_start, net, Signal::HIGH);
        // Falling edge
        scheduleEvent(cycle_start + half_period, net, Signal::LOW);
    }
}

// ─────────────────────────────────────────────────────
// Simulation execution
// ─────────────────────────────────────────────────────

void SimulationEngine::run(uint64_t max_time) {
    while (!event_queue_.empty()) {
        // Peek at the earliest event to see if we've exceeded the time limit
        if (event_queue_.top().time > max_time) {
            break;
        }

        uint64_t current_time = event_queue_.top().time;
        current_time_ = current_time;

        // 1. Gather all events at this exact timestamp
        std::vector<Event> current_events;
        while (!event_queue_.empty() && event_queue_.top().time == current_time) {
            current_events.push_back(event_queue_.top());
            event_queue_.pop();
            ++events_processed_;
        }

        // 2. Update nets and track which ones actually changed value
        std::vector<Net*> changed_nets;
        for (const auto& evt : current_events) {
            if (evt.target_net->getValue() != evt.new_value) {
                evt.target_net->setValue(evt.new_value);
                changed_nets.push_back(evt.target_net);

                // Notify observer (e.g., VCD writer) of the value change
                if (value_change_callback_) {
                    value_change_callback_(current_time_, evt.target_net, evt.new_value);
                }
            }
        }

        // 3. Find unique gates affected by these changes
        std::vector<Gate*> gates_to_evaluate;
        std::unordered_set<Gate*> unique_gates;
        for (Net* net : changed_nets) {
            auto it = fanout_map_.find(net);
            if (it != fanout_map_.end()) {
                for (Gate* gate : it->second) {
                    if (unique_gates.insert(gate).second) {
                        gates_to_evaluate.push_back(gate);
                    }
                }
            }
        }

        // 4. Evaluate each gate exactly once, scheduling output changes
        for (Gate* gate : gates_to_evaluate) {
            Net* output_net = gate->getOutput();
            if (!output_net) {
                continue;
            }

            Signal old_output = output_net->getValue();
            Signal new_output = gate->evaluate();

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
    value_change_callback_ = nullptr;
}

void SimulationEngine::setValueChangeCallback(ValueChangeCallback callback) {
    value_change_callback_ = std::move(callback);
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
