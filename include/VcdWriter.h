/**
 * @file VcdWriter.h
 * @brief Value Change Dump (VCD) waveform file writer.
 *
 * Generates IEEE 1364 VCD files for viewing simulation results
 * in waveform viewers such as GTKWave. Records signal transitions
 * on registered nets during simulation.
 */

#pragma once

#include "Signal.h"
#include "Net.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <cstdint>

/**
 * @class VcdWriter
 * @brief Writes simulation waveform data in VCD (Value Change Dump) format.
 *
 * Usage:
 *   1. Create a VcdWriter with an output filename
 *   2. Register nets to trace via addNet()
 *   3. Call writeHeader() to emit the VCD preamble
 *   4. Call recordChange() during simulation for each net value change
 *   5. Call close() to finalize the file
 */
class VcdWriter {
public:
    /**
     * @brief Construct a VCD writer.
     * @param filename  Output VCD file path.
     * @param timescale Timescale string (default: "1ns").
     */
    explicit VcdWriter(const std::string& filename,
                       const std::string& timescale = "1ns");

    /** @brief Destructor — closes the file if still open. */
    ~VcdWriter();

    // Non-copyable
    VcdWriter(const VcdWriter&) = delete;
    VcdWriter& operator=(const VcdWriter&) = delete;

    /**
     * @brief Register a net for VCD tracing.
     * @param net Pointer to the net to trace.
     *
     * Must be called before writeHeader().
     */
    void addNet(const Net* net);

    /**
     * @brief Write the VCD header (definitions, scope, variable declarations).
     *
     * Must be called after all nets are registered and before any
     * recordChange() calls.
     */
    void writeHeader();

    /**
     * @brief Record a signal change event.
     * @param time      Simulation time of the change.
     * @param net       Pointer to the net that changed.
     * @param new_value The new signal value.
     */
    void recordChange(uint64_t time, const Net* net, Signal new_value);

    /**
     * @brief Write initial values for all nets at time 0.
     *
     * Called automatically by writeHeader() using each net's current value.
     */
    void writeInitialValues();

    /**
     * @brief Close the VCD file.
     */
    void close();

    /**
     * @brief Check if the writer is active (file is open and header written).
     */
    bool isActive() const;

private:
    std::ofstream file_;
    std::string   timescale_;
    bool          header_written_;

    /// Ordered list of traced nets
    std::vector<const Net*> nets_;

    /// Map from Net pointer to its VCD identifier character
    std::unordered_map<const Net*, char> net_to_id_;

    /// Last timestamp written (to avoid duplicate #time lines)
    uint64_t last_time_;

    /// Next available VCD identifier character
    char next_id_;
};
