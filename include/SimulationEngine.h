/**
 * @file SimulationEngine.h
 * @brief Event-driven simulation engine for gate-level circuits.
 *
 * The SimulationEngine is the core of the simulator. It maintains:
 * - A priority queue (min-heap) of Events, processed in chronological order
 * - An unordered_map-based fanout table mapping each Net to the Gates it drives
 * - The current simulation time
 *
 * Simulation loop:
 *   1. Pop the earliest event from the queue
 *   2. Update the target net's value
 *   3. Look up all gates in the fanout of that net
 *   4. Evaluate each affected gate
 *   5. If a gate's computed output differs from its output net's current value,
 *      schedule a new event at (current_time + gate_propagation_delay)
 *   6. Repeat until the queue is empty or max_time is reached
 */

#pragma once

#include "Event.h"
#include "Signal.h"

#include <queue>
#include <vector>
#include <unordered_map>
#include <functional>
#include <cstdint>

class Net;
class Gate;

/**
 * @class SimulationEngine
 * @brief Event-driven simulation engine using a priority queue and fanout map.
 */
class SimulationEngine {
public:
    SimulationEngine();
    ~SimulationEngine();

    // ── Circuit construction ──────────────────────────

    /**
     * @brief Register a net with the engine.
     * @param net Pointer to the Net (engine does NOT take ownership).
     */
    void addNet(Net* net);

    /**
     * @brief Register a gate with the engine.
     * @param gate Pointer to the Gate (engine does NOT take ownership).
     */
    void addGate(Gate* gate);

    /**
     * @brief Build the fanout map from registered gates.
     *
     * Must be called after all gates and nets have been added, and before
     * run(). Iterates over each gate's input nets and populates the
     * unordered_map<Net*, vector<Gate*>> fanout table.
     */
    void buildFanoutMap();

    // ── Event scheduling ──────────────────────────────

    /**
     * @brief Schedule a signal change event.
     * @param time      Simulation time at which the event occurs.
     * @param net       Target net whose value will change.
     * @param value     The new signal value.
     */
    void scheduleEvent(uint64_t time, Net* net, Signal value);

    // ── Simulation control ────────────────────────────

    /**
     * @brief Run the simulation until the event queue is empty or max_time.
     * @param max_time  Maximum simulation time to process (default: unlimited).
     */
    void run(uint64_t max_time = UINT64_MAX);

    /**
     * @brief Reset the engine state (clears events, time, and fanout map).
     */
    void reset();

    // ── Accessors ─────────────────────────────────────

    /** @brief Get the current simulation time. */
    uint64_t getCurrentTime() const;

    /** @brief Get the total number of events processed in the last run. */
    uint64_t getEventsProcessed() const;

    /** @brief Get the registered nets. */
    const std::vector<Net*>& getNets() const;

    /** @brief Get the registered gates. */
    const std::vector<Gate*>& getGates() const;

private:
    /// Min-heap: earliest event first
    std::priority_queue<Event, std::vector<Event>, std::greater<Event>> event_queue_;

    /// Fanout map: Net* -> vector of Gates that have this net as an input
    std::unordered_map<Net*, std::vector<Gate*>> fanout_map_;

    /// All registered nets and gates
    std::vector<Net*> nets_;
    std::vector<Gate*> gates_;

    /// Current simulation time
    uint64_t current_time_;

    /// Counter for events processed
    uint64_t events_processed_;
};
