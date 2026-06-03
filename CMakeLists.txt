cmake_minimum_required(VERSION 3.14)
project(GateLevelSimulator VERSION 1.0.0 LANGUAGES CXX)

# Force global preprocessor definitions for all targets (including googletest)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -U__STRICT_ANSI__")

# ──────────────────────────────────────────────
# C++ Standard
# ──────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# ──────────────────────────────────────────────
# Compiler warnings
# ──────────────────────────────────────────────
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ──────────────────────────────────────────────
# Core simulation library
# ──────────────────────────────────────────────
add_library(gate_sim_lib
    src/Net.cpp
    src/Gate.cpp
    src/SimulationEngine.cpp
    src/VcdWriter.cpp
    src/gates/AndGate.cpp
    src/gates/OrGate.cpp
    src/gates/NotGate.cpp
    src/gates/XorGate.cpp
    src/gates/MuxGate.cpp
    src/gates/DFlipFlop.cpp
)

target_include_directories(gate_sim_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ──────────────────────────────────────────────
# Main executable
# ──────────────────────────────────────────────
add_executable(gate_sim src/main.cpp)
target_link_libraries(gate_sim PRIVATE gate_sim_lib)

# ──────────────────────────────────────────────
# Tests (enabled in a later commit)
# ──────────────────────────────────────────────
option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
