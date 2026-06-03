/**
 * @file VcdWriter.cpp
 * @brief Implementation of IEEE 1364 VCD file writer.
 *
 * Generates waveform dumps compatible with GTKWave and other
 * VCD-compatible viewers. Each registered net is assigned a
 * unique single-character identifier for compact output.
 */

#include "VcdWriter.h"

#include <iostream>
#include <ctime>
#include <sstream>

// ─────────────────────────────────────────────────────
// Construction / Destruction
// ─────────────────────────────────────────────────────

VcdWriter::VcdWriter(const std::string& filename, const std::string& timescale)
    : timescale_(timescale)
    , header_written_(false)
    , last_time_(UINT64_MAX)
    , next_id_('!')
{
    file_.open(filename, std::ios::out | std::ios::trunc);
    if (!file_.is_open()) {
        std::cerr << "[VcdWriter] ERROR: Could not open file: " << filename << std::endl;
    }
}

VcdWriter::~VcdWriter() {
    close();
}

// ─────────────────────────────────────────────────────
// Net registration
// ─────────────────────────────────────────────────────

void VcdWriter::addNet(const Net* net) {
    if (header_written_) {
        std::cerr << "[VcdWriter] WARNING: Cannot add nets after header is written." << std::endl;
        return;
    }
    if (net_to_id_.find(net) != net_to_id_.end()) {
        return;  // Already registered
    }

    char id = next_id_++;
    nets_.push_back(net);
    net_to_id_[net] = id;
}

// ─────────────────────────────────────────────────────
// Header
// ─────────────────────────────────────────────────────

void VcdWriter::writeHeader() {
    if (!file_.is_open()) return;

    // Date
    std::time_t now = std::time(nullptr);
    file_ << "$date" << std::endl;
    file_ << "   " << std::ctime(&now);
    file_ << "$end" << std::endl;

    // Version
    file_ << "$version" << std::endl;
    file_ << "   Gate-Level Event-Driven Logic Simulator v1.0" << std::endl;
    file_ << "$end" << std::endl;

    // Timescale
    file_ << "$timescale" << std::endl;
    file_ << "   " << timescale_ << std::endl;
    file_ << "$end" << std::endl;

    // Scope and variable declarations
    file_ << "$scope module circuit $end" << std::endl;

    for (const Net* net : nets_) {
        char id = net_to_id_[net];
        file_ << "$var wire 1 " << id << " " << net->getName() << " $end" << std::endl;
    }

    file_ << "$upscope $end" << std::endl;
    file_ << "$enddefinitions $end" << std::endl;

    header_written_ = true;

    // Write initial values at time 0
    writeInitialValues();
}

// ─────────────────────────────────────────────────────
// Initial values
// ─────────────────────────────────────────────────────

void VcdWriter::writeInitialValues() {
    if (!file_.is_open() || !header_written_) return;

    file_ << "$dumpvars" << std::endl;
    for (const Net* net : nets_) {
        char id = net_to_id_[net];
        file_ << signalToChar(net->getValue()) << id << std::endl;
    }
    file_ << "$end" << std::endl;

    last_time_ = 0;
}

// ─────────────────────────────────────────────────────
// Record value changes
// ─────────────────────────────────────────────────────

void VcdWriter::recordChange(uint64_t time, const Net* net, Signal new_value) {
    if (!file_.is_open() || !header_written_) return;

    auto it = net_to_id_.find(net);
    if (it == net_to_id_.end()) return;  // Net not registered

    // Write timestamp if it's a new time point
    if (time != last_time_) {
        file_ << "#" << time << std::endl;
        last_time_ = time;
    }

    // Write value change
    file_ << signalToChar(new_value) << it->second << std::endl;
}

// ─────────────────────────────────────────────────────
// Close
// ─────────────────────────────────────────────────────

void VcdWriter::close() {
    if (file_.is_open()) {
        file_.flush();
        file_.close();
    }
}

bool VcdWriter::isActive() const {
    return file_.is_open() && header_written_;
}
