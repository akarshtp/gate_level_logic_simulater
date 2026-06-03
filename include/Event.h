/**
 * @file Event.h
 * @brief Simulation event representation for the event-driven engine.
 *
 * An Event represents a scheduled signal change on a Net at a specific
 * simulation time. Events are ordered by time for priority queue processing.
 */

#pragma once

#include "Signal.h"
#include <cstdint>

class Net;  // Forward declaration

/**
 * @struct Event
 * @brief A scheduled signal transition on a target net.
 *
 * The event-driven simulator processes events in chronological order.
 * Each event specifies when a net should transition to a new value.
 */
struct Event {
    uint64_t time;         ///< Simulation time (in time units) when this event occurs
    Net*     target_net;   ///< Pointer to the net whose value will change
    Signal   new_value;    ///< The new signal value to assign

    /**
     * @brief Greater-than comparison for min-heap ordering.
     *
     * Used by std::priority_queue with std::greater<Event> to create
     * a min-heap where the earliest event is processed first.
     *
     * @param other The event to compare against.
     * @return true if this event occurs later than @p other.
     */
    bool operator>(const Event& other) const {
        return time > other.time;
    }

    /**
     * @brief Equality comparison.
     * @param other The event to compare against.
     * @return true if both events target the same net at the same time with the same value.
     */
    bool operator==(const Event& other) const {
        return time == other.time &&
               target_net == other.target_net &&
               new_value == other.new_value;
    }
};
