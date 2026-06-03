/**
 * @file main.cpp
 * @brief Entry point for the Gate-Level Event-Driven Logic Simulator.
 *
 * Demonstrates a working combinational circuit:
 *   Circuit: OUT = (A AND B) XOR C
 *   - AND gate computes A·B
 *   - XOR gate computes (A·B) ⊕ C
 *
 * Also demonstrates a half-adder circuit:
 *   Sum   = A XOR B
 *   Carry = A AND B
 *
 * Generates VCD waveform files for viewing in GTKWave.
 */

#include "Signal.h"
#include "Net.h"
#include "Gate.h"
#include "Event.h"
#include "SimulationEngine.h"
#include "VcdWriter.h"

#include "gates/AndGate.h"
#include "gates/OrGate.h"
#include "gates/NotGate.h"
#include "gates/XorGate.h"
#include "gates/MuxGate.h"
#include "gates/DFlipFlop.h"

#include <iostream>
#include <iomanip>
#include <string>

// ─────────────────────────────────────────────────────
// Utility: print a horizontal rule
// ─────────────────────────────────────────────────────
static void printRule() {
    std::cout << "  ───────────────────────────────────────────────" << std::endl;
}

static void printBanner() {
    std::cout << R"(
  ╔══════════════════════════════════════════════════════════╗
  ║   Gate-Level Event-Driven Logic Simulator v1.0          ║
  ║   ─────────────────────────────────────────────────     ║
  ║   C++ | STL priority_queue | OOP Virtual Dispatch       ║
  ╚══════════════════════════════════════════════════════════╝
)" << std::endl;
}

// ─────────────────────────────────────────────────────
// Demo 1: (A AND B) XOR C
// ─────────────────────────────────────────────────────
static void demoCombinatorialCircuit() {
    std::cout << "  Demo 1: Combinational Circuit — OUT = (A AND B) XOR C" << std::endl;
    printRule();
    std::cout << std::endl;
    std::cout << "  Schematic:" << std::endl;
    std::cout << "    A ──┐" << std::endl;
    std::cout << "        ├── [AND] ── W ──┐" << std::endl;
    std::cout << "    B ──┘                 ├── [XOR] ── OUT" << std::endl;
    std::cout << "    C ─────────────────────┘" << std::endl;
    std::cout << std::endl;

    // Create nets
    Net net_a("A", Signal::LOW);
    Net net_b("B", Signal::LOW);
    Net net_c("C", Signal::LOW);
    Net net_w("W", Signal::UNKNOWN);   // Internal wire (AND output)
    Net net_out("OUT", Signal::UNKNOWN);

    // Create gates with propagation delays
    AndGate and_gate("U1_AND", 2);   // 2ns delay
    XorGate xor_gate("U2_XOR", 3);   // 3ns delay

    // Wire up: AND(A, B) -> W
    and_gate.addInput(&net_a);
    and_gate.addInput(&net_b);
    and_gate.setOutput(&net_w);

    // Wire up: XOR(W, C) -> OUT
    xor_gate.addInput(&net_w);
    xor_gate.addInput(&net_c);
    xor_gate.setOutput(&net_out);

    // Create simulation engine
    SimulationEngine engine;
    engine.addNet(&net_a);
    engine.addNet(&net_b);
    engine.addNet(&net_c);
    engine.addNet(&net_w);
    engine.addNet(&net_out);
    engine.addGate(&and_gate);
    engine.addGate(&xor_gate);
    engine.buildFanoutMap();

    // Setup VCD writer
    VcdWriter vcd("demo1_combinational.vcd");
    vcd.addNet(&net_a);
    vcd.addNet(&net_b);
    vcd.addNet(&net_c);
    vcd.addNet(&net_w);
    vcd.addNet(&net_out);
    vcd.writeHeader();

    engine.setValueChangeCallback(
        [&vcd](uint64_t time, const Net* net, Signal value) {
            vcd.recordChange(time, net, value);
        }
    );

    // Schedule input stimulus
    // Test case 1: A=1, B=1, C=0 → AND=1, XOR=1⊕0=1
    engine.scheduleEvent(0,  &net_a, Signal::HIGH);
    engine.scheduleEvent(0,  &net_b, Signal::HIGH);
    engine.scheduleEvent(0,  &net_c, Signal::LOW);

    // Test case 2: C=1 at t=10 → AND=1, XOR=1⊕1=0
    engine.scheduleEvent(10, &net_c, Signal::HIGH);

    // Test case 3: A=0 at t=20 → AND=0, XOR=0⊕1=1
    engine.scheduleEvent(20, &net_a, Signal::LOW);

    // Test case 4: B=0 at t=30 → AND=0, XOR=0⊕1=1
    engine.scheduleEvent(30, &net_b, Signal::LOW);

    // Test case 5: C=0 at t=40 → AND=0, XOR=0⊕0=0
    engine.scheduleEvent(40, &net_c, Signal::LOW);

    // Run simulation
    engine.run();
    vcd.close();

    // Print results table
    std::cout << "  Simulation Results (after all events settle):" << std::endl;
    std::cout << "  ┌──────┬──────┬──────┬──────┬──────┐" << std::endl;
    std::cout << "  │  A   │  B   │  C   │  W   │ OUT  │" << std::endl;
    std::cout << "  ├──────┼──────┼──────┼──────┼──────┤" << std::endl;
    std::cout << "  │  " << signalToChar(net_a.getValue())
              << "   │  " << signalToChar(net_b.getValue())
              << "   │  " << signalToChar(net_c.getValue())
              << "   │  " << signalToChar(net_w.getValue())
              << "   │  " << signalToChar(net_out.getValue())
              << "   │" << std::endl;
    std::cout << "  └──────┴──────┴──────┴──────┴──────┘" << std::endl;
    std::cout << std::endl;
    std::cout << "  Events processed: " << engine.getEventsProcessed() << std::endl;
    std::cout << "  Final time: " << engine.getCurrentTime() << " ns" << std::endl;
    std::cout << "  VCD output: demo1_combinational.vcd" << std::endl;
    std::cout << std::endl;
}

// ─────────────────────────────────────────────────────
// Demo 2: Half Adder (A + B)
// ─────────────────────────────────────────────────────
static void demoHalfAdder() {
    std::cout << "  Demo 2: Half Adder — Sum = A XOR B, Carry = A AND B" << std::endl;
    printRule();
    std::cout << std::endl;
    std::cout << "  Schematic:" << std::endl;
    std::cout << "    A ──┬── [XOR] ── Sum" << std::endl;
    std::cout << "        │" << std::endl;
    std::cout << "    B ──┼── [AND] ── Carry" << std::endl;
    std::cout << std::endl;

    // Create nets (start UNKNOWN so first stimulus always triggers evaluation)
    Net net_a("A", Signal::UNKNOWN);
    Net net_b("B", Signal::UNKNOWN);
    Net net_sum("Sum", Signal::UNKNOWN);
    Net net_carry("Carry", Signal::UNKNOWN);

    // Create gates
    XorGate xor_sum("U1_XOR_SUM", 2);
    AndGate and_carry("U2_AND_CARRY", 2);

    // Wire up
    xor_sum.addInput(&net_a);
    xor_sum.addInput(&net_b);
    xor_sum.setOutput(&net_sum);

    and_carry.addInput(&net_a);
    and_carry.addInput(&net_b);
    and_carry.setOutput(&net_carry);

    // Engine
    SimulationEngine engine;
    engine.addNet(&net_a);
    engine.addNet(&net_b);
    engine.addNet(&net_sum);
    engine.addNet(&net_carry);
    engine.addGate(&xor_sum);
    engine.addGate(&and_carry);
    engine.buildFanoutMap();

    // VCD
    VcdWriter vcd("demo2_half_adder.vcd");
    vcd.addNet(&net_a);
    vcd.addNet(&net_b);
    vcd.addNet(&net_sum);
    vcd.addNet(&net_carry);
    vcd.writeHeader();

    engine.setValueChangeCallback(
        [&vcd](uint64_t time, const Net* net, Signal value) {
            vcd.recordChange(time, net, value);
        }
    );

    // Truth table exhaustive test
    std::cout << "  Full truth table simulation:" << std::endl;
    std::cout << "  ┌──────┬──────┬──────┬───────┐" << std::endl;
    std::cout << "  │  A   │  B   │ Sum  │ Carry │" << std::endl;
    std::cout << "  ├──────┼──────┼──────┼───────┤" << std::endl;

    Signal inputs[2] = {Signal::LOW, Signal::HIGH};
    uint64_t time = 0;

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            engine.scheduleEvent(time, &net_a, inputs[i]);
            engine.scheduleEvent(time, &net_b, inputs[j]);
            engine.run(time + 15);  // Let events settle (> 2x gate delay)

            std::cout << "  │  " << signalToChar(net_a.getValue())
                      << "   │  " << signalToChar(net_b.getValue())
                      << "   │  " << signalToChar(net_sum.getValue())
                      << "   │   " << signalToChar(net_carry.getValue())
                      << "   │" << std::endl;
            time += 30;
        }
    }

    std::cout << "  └──────┴──────┴──────┴───────┘" << std::endl;
    std::cout << std::endl;
    std::cout << "  Events processed: " << engine.getEventsProcessed() << std::endl;
    std::cout << "  VCD output: demo2_half_adder.vcd" << std::endl;
    std::cout << std::endl;

    vcd.close();
}

// ─────────────────────────────────────────────────────
// Demo 3: NOT + OR gate chain
// ─────────────────────────────────────────────────────
static void demoNotOrChain() {
    std::cout << "  Demo 3: Gate Chain — OUT = NOT(A) OR B" << std::endl;
    printRule();
    std::cout << std::endl;
    std::cout << "  Schematic:" << std::endl;
    std::cout << "    A ── [NOT] ── A_INV ──┐" << std::endl;
    std::cout << "                          ├── [OR] ── OUT" << std::endl;
    std::cout << "    B ──────────────────────┘" << std::endl;
    std::cout << std::endl;

    Net net_a("A", Signal::UNKNOWN);
    Net net_b("B", Signal::UNKNOWN);
    Net net_a_inv("A_INV", Signal::UNKNOWN);
    Net net_out("OUT", Signal::UNKNOWN);

    NotGate not_gate("U1_NOT", 1);
    OrGate  or_gate("U2_OR", 2);

    not_gate.addInput(&net_a);
    not_gate.setOutput(&net_a_inv);

    or_gate.addInput(&net_a_inv);
    or_gate.addInput(&net_b);
    or_gate.setOutput(&net_out);

    SimulationEngine engine;
    engine.addNet(&net_a);
    engine.addNet(&net_b);
    engine.addNet(&net_a_inv);
    engine.addNet(&net_out);
    engine.addGate(&not_gate);
    engine.addGate(&or_gate);
    engine.buildFanoutMap();

    // Test: A=1, B=0 → NOT(1)=0, 0 OR 0 = 0
    engine.scheduleEvent(0, &net_a, Signal::HIGH);
    engine.scheduleEvent(0, &net_b, Signal::LOW);
    engine.run(20);

    std::cout << "  A=1, B=0 → NOT(A)=" << signalToChar(net_a_inv.getValue())
              << ", OUT=" << signalToChar(net_out.getValue()) << std::endl;

    // Test: A=0, B=0 → NOT(0)=1, 1 OR 0 = 1
    engine.scheduleEvent(30, &net_a, Signal::LOW);
    engine.run(60);

    std::cout << "  A=0, B=0 → NOT(A)=" << signalToChar(net_a_inv.getValue())
              << ", OUT=" << signalToChar(net_out.getValue()) << std::endl;

    // Test: A=0, B=1 → NOT(0)=1, 1 OR 1 = 1
    engine.scheduleEvent(70, &net_b, Signal::HIGH);
    engine.run(100);

    std::cout << "  A=0, B=1 → NOT(A)=" << signalToChar(net_a_inv.getValue())
              << ", OUT=" << signalToChar(net_out.getValue()) << std::endl;

    std::cout << std::endl;
    std::cout << "  Events processed: " << engine.getEventsProcessed() << std::endl;
    std::cout << std::endl;
}

// ─────────────────────────────────────────────────────
// Demo 4: Sequential Loadable Register
// ─────────────────────────────────────────────────────
static void demoSequentialCircuit() {
    std::cout << "  Demo 4: Sequential Loadable Register (MUX + DFF)" << std::endl;
    printRule();
    std::cout << std::endl;
    std::cout << "  Schematic:" << std::endl;
    std::cout << "             ┌──────────────────────┐" << std::endl;
    std::cout << "             │                      │" << std::endl;
    std::cout << "             ▼                      │" << std::endl;
    std::cout << "    Q ───── [D0]                    │" << std::endl;
    std::cout << "            [MUX] ── Next_D ── [D]  │" << std::endl;
    std::cout << "    D ───── [D1]               [DFF]┴── Q" << std::endl;
    std::cout << "            [SEL]              [CLK]" << std::endl;
    std::cout << "              ▲                  ▲" << std::endl;
    std::cout << "              │                  │" << std::endl;
    std::cout << "             Load               CLK" << std::endl;
    std::cout << std::endl;

    // Create nets
    Net net_d("D", Signal::LOW);
    Net net_load("Load", Signal::LOW);
    Net net_clk("CLK", Signal::LOW);
    Net net_next_d("Next_D", Signal::UNKNOWN);
    Net net_q("Q", Signal::UNKNOWN);

    // Create gates
    // inputs: Q (D0), D (D1), Load (SEL)
    MuxGate mux("U1_MUX", 2);
    // inputs: Next_D (D), CLK (CLK)
    DFlipFlop dff("U2_DFF", 3);

    // Wire up MUX
    mux.addInput(&net_q);
    mux.addInput(&net_d);
    mux.addInput(&net_load);
    mux.setOutput(&net_next_d);

    // Wire up DFF
    dff.addInput(&net_next_d);
    dff.addInput(&net_clk);
    dff.setOutput(&net_q);

    // Engine
    SimulationEngine engine;
    engine.addNet(&net_d);
    engine.addNet(&net_load);
    engine.addNet(&net_clk);
    engine.addNet(&net_next_d);
    engine.addNet(&net_q);
    engine.addGate(&mux);
    engine.addGate(&dff);
    engine.buildFanoutMap();

    // VCD Setup
    VcdWriter vcd("demo3_sequential.vcd");
    vcd.addNet(&net_d);
    vcd.addNet(&net_load);
    vcd.addNet(&net_clk);
    vcd.addNet(&net_next_d);
    vcd.addNet(&net_q);
    vcd.writeHeader();

    engine.setValueChangeCallback(
        [&vcd](uint64_t time, const Net* net, Signal value) {
            vcd.recordChange(time, net, value);
        }
    );

    // Generate periodic clock: 20ns period, 6 cycles, starting at t=5
    engine.generateClock(&net_clk, 20, 6, 5);

    // Input stimulus
    // At t=0: D=LOW, Load=LOW. (MUX selects Q=UNKNOWN -> Next_D = UNKNOWN)
    
    // At t=10: D=HIGH, Load=HIGH. (MUX selects D=HIGH -> Next_D = HIGH)
    // This will be latched at Cycle 1 rising edge (t=25).
    engine.scheduleEvent(10, &net_d, Signal::HIGH);
    engine.scheduleEvent(10, &net_load, Signal::HIGH);

    // At t=40: D=LOW, Load=LOW. (MUX selects Q=HIGH -> Next_D = HIGH)
    // At Cycle 2 rising edge (t=45), DFF latches Next_D = HIGH (value doesn't change, stays HIGH).
    engine.scheduleEvent(40, &net_d, Signal::LOW);
    engine.scheduleEvent(40, &net_load, Signal::LOW);

    // At t=70: D=LOW, Load=HIGH. (MUX selects D=LOW -> Next_D = LOW)
    // This will be latched at Cycle 4 rising edge (t=85).
    engine.scheduleEvent(70, &net_load, Signal::HIGH);

    // At t=100: Load=LOW. (MUX selects Q=LOW -> Next_D = LOW)
    // At Cycle 5 rising edge (t=105), DFF latches Next_D = LOW (stays LOW).
    engine.scheduleEvent(100, &net_load, Signal::LOW);

    std::cout << "  Simulating loadable register over 120 ns..." << std::endl;
    std::cout << "  ┌──────┬──────┬──────┬────────┬───┐" << std::endl;
    std::cout << "  │ Time │  D   │ Load │ Next_D │ Q │" << std::endl;
    std::cout << "  ├──────┼──────┼──────┼────────┼───┤" << std::endl;

    // Run simulation step by step to print truth-like table
    for (uint64_t step_time = 0; step_time <= 120; step_time += 10) {
        engine.run(step_time);
        std::cout << "  │ " << std::setw(4) << step_time << " │  "
                  << signalToChar(net_d.getValue()) << "   │  "
                  << signalToChar(net_load.getValue()) << "   │   "
                  << signalToChar(net_next_d.getValue()) << "    │ "
                  << signalToChar(net_q.getValue()) << " │" << std::endl;
    }

    std::cout << "  └──────┴──────┴──────┴────────┴───┘" << std::endl;
    std::cout << std::endl;
    std::cout << "  Events processed: " << engine.getEventsProcessed() << std::endl;
    std::cout << "  VCD output: demo3_sequential.vcd" << std::endl;
    std::cout << std::endl;

    vcd.close();
}

// ─────────────────────────────────────────────────────
// Main
// ─────────────────────────────────────────────────────
int main() {
    printBanner();

    demoCombinatorialCircuit();
    std::cout << std::endl;

    demoHalfAdder();
    std::cout << std::endl;

    demoNotOrChain();
    std::cout << std::endl;

    demoSequentialCircuit();
    std::cout << std::endl;

    std::cout << "  All demos completed successfully!" << std::endl;
    std::cout << std::endl;

    return 0;
}
