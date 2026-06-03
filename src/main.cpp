/**
 * @file main.cpp
 * @brief Entry point for the Gate-Level Event-Driven Logic Simulator.
 *
 * This is the initial scaffold. Gate implementations and demo circuits
 * will be added in subsequent commits.
 */

#include "Signal.h"
#include "Net.h"
#include "Gate.h"
#include "Event.h"
#include "SimulationEngine.h"

#include <iostream>
#include <string>

static void printBanner() {
    std::cout << R"(
  ╔══════════════════════════════════════════════════════════╗
  ║   Gate-Level Event-Driven Logic Simulator v1.0          ║
  ║   ─────────────────────────────────────────────────     ║
  ║   C++ | STL priority_queue | OOP Virtual Dispatch       ║
  ╚══════════════════════════════════════════════════════════╝
)" << std::endl;
}

static void printArchitecture() {
    std::cout << "  Architecture Overview:" << std::endl;
    std::cout << "  ───────────────────────────────────────" << std::endl;
    std::cout << std::endl;
    std::cout << "  Signal     : 3-valued logic (LOW / HIGH / UNKNOWN)" << std::endl;
    std::cout << "  Net        : Signal-carrying wire with fanout tracking" << std::endl;
    std::cout << "  Gate       : Abstract base with virtual evaluate()" << std::endl;
    std::cout << "  Event      : Scheduled signal transition {time, net, value}" << std::endl;
    std::cout << "  Engine     : priority_queue<Event> + unordered_map fanout" << std::endl;
    std::cout << std::endl;
    std::cout << "  Supported Primitives:" << std::endl;
    std::cout << "    • AND, OR, NOT, XOR  (combinational)" << std::endl;
    std::cout << "    • MUX 2:1            (combinational)" << std::endl;
    std::cout << "    • D Flip-Flop        (sequential, rising-edge)" << std::endl;
    std::cout << std::endl;
}

int main() {
    printBanner();
    printArchitecture();

    // Quick sanity check: create nets and verify signal values
    Net net_a("A", Signal::LOW);
    Net net_b("B", Signal::HIGH);
    Net net_x("X");  // defaults to UNKNOWN

    std::cout << "  Signal Sanity Check:" << std::endl;
    std::cout << "    Net '" << net_a.getName() << "' = " << signalToString(net_a.getValue()) << std::endl;
    std::cout << "    Net '" << net_b.getName() << "' = " << signalToString(net_b.getValue()) << std::endl;
    std::cout << "    Net '" << net_x.getName() << "' = " << signalToString(net_x.getValue()) << std::endl;
    std::cout << std::endl;

    // Engine skeleton
    SimulationEngine engine;
    engine.addNet(&net_a);
    engine.addNet(&net_b);
    engine.addNet(&net_x);

    std::cout << "  Engine initialized with " << engine.getNets().size()
              << " nets, " << engine.getGates().size() << " gates." << std::endl;
    std::cout << std::endl;
    std::cout << "  [INFO] Gate implementations will be added in the next commit." << std::endl;
    std::cout << "  [INFO] Demo circuits coming soon!" << std::endl;
    std::cout << std::endl;

    return 0;
}
