# Gate-Level Event-Driven Logic Simulator

[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B17)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)](#)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Framework](https://img.shields.io/badge/Testing-Google%20Test-red.svg)](https://github.com/google/googletest)

A high-performance, cross-platform, event-driven digital logic simulator written in C++. The simulator models combinational and sequential logic primitives using object-oriented principles, virtual dispatch, and event scheduling. It supports **3-valued logic** (`LOW`, `HIGH`, `UNKNOWN`) and configurable propagation delays per gate.

Under the hood, the engine employs a **2-phase event propagation mechanism** that groups simultaneous signal transitions, preventing evaluation glitches and race conditions. It also writes standard **VCD (Value Change Dump)** waveforms for interactive analysis in visualization tools like GTKWave.

---

## Architecture Overview

The simulator avoids wasteful cycle-based evaluation by only re-evaluating gates when their inputs experience a value transition (event-driven). 

```text
       Event Queue (std::priority_queue min-heap by time)
                           │
                           ▼ (Pop all events at current t)
                [ Update Net Values ]
                           │
                           ▼ (Track changed nets)
           [ Fanout Map (std::unordered_map) ]
                           │
                           ▼ (Gather affected unique gates)
               [ Evaluate Gates (OOP) ] ───► Virtual Dispatch
                           │
                           ▼ (Check if output changed)
               [ Schedule New Events ]
                    (t + gate_delay)
```

---

## Supported Logic Primitives

| Gate Type | Inputs Order | Function | 3-Valued Logic Notes |
|---|---|---|---|
| **AND** | `in_0, in_1, ...` | `A · B · ...` | Short-circuits to `LOW` if any input is `LOW`. Returns `UNKNOWN` if any input is `UNKNOWN` and none are `LOW`. |
| **OR** | `in_0, in_1, ...` | `A + B + ...` | Short-circuits to `HIGH` if any input is `HIGH`. Returns `UNKNOWN` if any input is `UNKNOWN` and none are `HIGH`. |
| **NOT** | `in_0` | `~A` | Inverts `LOW` $\leftrightarrow$ `HIGH`. Passes `UNKNOWN` $\rightarrow$ `UNKNOWN`. |
| **XOR** | `in_0, in_1` | `A ⊕ B` | Returns `UNKNOWN` if any input is `UNKNOWN`. |
| **MUX** | `D0, D1, SEL` | `SEL ? D1 : D0` | If `SEL` is `UNKNOWN` but both inputs `D0 == D1`, the output settles to that common value (prevents X-propagation). |
| **DFF** | `D, CLK` | Sequential latch | Rising-edge triggered (`CLK` transitions `LOW` $\rightarrow$ `HIGH`). Stores internal state. |

---

## Directory Structure

```text
Gate_Level Logic_Simulator/
├── CMakeLists.txt              # Root CMake configuration
├── LICENSE                     # MIT License
├── README.md                   # Project documentation
├── include/                    # Header files
│   ├── Event.h                 # Event structural definition
│   ├── Gate.h                  # Abstract base Gate class
│   ├── Net.h                   # Wire/Signal class
│   ├── Signal.h                # Signal enum & helper functions
│   ├── SimulationEngine.h      # Core simulation event loop
│   ├── VcdWriter.h             # VCD waveform recording utility
│   └── gates/                  # Concrete gate definitions
│       ├── AndGate.h
│       ├── OrGate.h
│       ├── NotGate.h
│       ├── XorGate.h
│       ├── MuxGate.h
│       └── DFlipFlop.h
├── src/                        # Source files
│   ├── Gate.cpp
│   ├── Net.cpp
│   ├── SimulationEngine.cpp
│   ├── VcdWriter.cpp
│   ├── main.cpp                # Run-loop demonstration program
│   └── gates/
│       ├── AndGate.cpp
│       ├── OrGate.cpp
│       ├── NotGate.cpp
│       ├── XorGate.cpp
│       ├── MuxGate.cpp
│       └── DFlipFlop.cpp
└── tests/                      # Automated test suite
    ├── CMakeLists.txt          # FetchContent GoogleTest configuration
    ├── test_gates.cpp          # Primitives unit tests
    ├── test_flipflop.cpp       # Sequential edge-triggering unit tests
    ├── test_simulation.cpp     # Multi-gate circuit integration tests
    └── test_stress.cpp         # 500+ gate ripple-carry adder stress test
```

---

## Installation & Compilation

### Prerequisites
*   A C++ compliant compiler (GCC, Clang, or MSVC)
*   **CMake** (version 3.14 or higher)

### Build Instructions (Windows & Linux)

To compile the library, main demos, and tests, execute:

```bash
# 1. Create a build directory and configure the project
cmake -B build -DCMAKE_BUILD_TYPE=Release

# 2. Compile all targets
cmake --build build
```

*Note for MinGW users on Windows: If you use MinGW, build using the MinGW Makefile generator:*
```bash
cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

---

## Running the Simulator

### 1. Demos
To run the included circuit demonstrations (Combinational circuit, Half Adder truth table, Gate chain, and Sequential Loadable Register):
```bash
# Windows
.\build\gate_sim.exe

# Linux/macOS
./build/gate_sim
```

### 2. Waveform Visualization (VCD)
The demo execution automatically generates three waveform files in the root folder:
*   `demo1_combinational.vcd`
*   `demo2_half_adder.vcd`
*   `demo3_sequential.vcd`

You can open these files in **GTKWave** to view the signal graphs:
1. Open GTKWave.
2. Go to **File** $\rightarrow$ **Open New Tab** and select a `.vcd` file.
3. Select the signals from the hierarchy tree on the left and drag them to the waveform viewer.

---

## Running the Test Suite

We use **Google Test** for verifying circuit behavior. CMake automatically fetches and builds the library locally in the background.

To run the unit and integration tests:
```bash
# Windows
.\build\tests\gate_sim_tests.exe

# Linux/macOS
./build/tests/gate_sim_tests
```

### Stress Test (500+ Gates)
The test suite includes a stress test ([test_stress.cpp](tests/test_stress.cpp)) that programmatically constructs a **105-bit Ripple-Carry Adder** containing **525 gates**. 
This verifies:
*   Propagation delay cascading across 210 gate levels.
*   Correct binary arithmetic outputs.
*   Event queue performance under heavy transition activity.
