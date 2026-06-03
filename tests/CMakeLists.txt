# ──────────────────────────────────────────────
# Google Test Suite Configuration
# ──────────────────────────────────────────────

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.8.1.zip
)

# Prevent overriding the parent project's settings, and disable pthreads
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(gtest_disable_pthreads ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(gate_sim_tests
    test_gates.cpp
    test_simulation.cpp
    test_flipflop.cpp
    test_stress.cpp
)

target_link_libraries(gate_sim_tests PRIVATE
    gate_sim_lib
    gtest_main
)

# Enable automatic test discovery
include(GoogleTest)
gtest_discover_tests(gate_sim_tests)
