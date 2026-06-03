#include <gtest/gtest.h>
#include "SimulationEngine.h"
#include "Net.h"
#include "Signal.h"
#include "gates/AndGate.h"
#include "gates/OrGate.h"
#include "gates/XorGate.h"

#include <vector>
#include <memory>
#include <string>

TEST(StressTest, RippleCarryAdder525Gates) {
    const int num_bits = 105; // 105 * 5 gates/FA = 525 gates
    SimulationEngine engine;

    // Vectors to hold ownership of nets and gates
    std::vector<std::unique_ptr<Net>> all_nets;
    std::vector<std::unique_ptr<Gate>> all_gates;

    // Helper functions for creation
    auto make_net = [&](const std::string& name, Signal val) -> Net* {
        all_nets.push_back(std::make_unique<Net>(name, val));
        Net* ptr = all_nets.back().get();
        engine.addNet(ptr);
        return ptr;
    };

    auto add_xor = [&](const std::string& name, Net* in0, Net* in1, Net* out) {
        all_gates.push_back(std::make_unique<XorGate>(name, 2));
        Gate* ptr = all_gates.back().get();
        ptr->addInput(in0);
        ptr->addInput(in1);
        ptr->setOutput(out);
        engine.addGate(ptr);
    };

    auto add_and = [&](const std::string& name, Net* in0, Net* in1, Net* out) {
        all_gates.push_back(std::make_unique<AndGate>(name, 2));
        Gate* ptr = all_gates.back().get();
        ptr->addInput(in0);
        ptr->addInput(in1);
        ptr->setOutput(out);
        engine.addGate(ptr);
    };

    auto add_or = [&](const std::string& name, Net* in0, Net* in1, Net* out) {
        all_gates.push_back(std::make_unique<OrGate>(name, 2));
        Gate* ptr = all_gates.back().get();
        ptr->addInput(in0);
        ptr->addInput(in1);
        ptr->setOutput(out);
        engine.addGate(ptr);
    };

    // Inputs & outputs arrays
    std::vector<Net*> net_a(num_bits);
    std::vector<Net*> net_b(num_bits);
    std::vector<Net*> net_sum(num_bits);
    std::vector<Net*> net_carry(num_bits + 1); // carry[0] is C_in, carry[num_bits] is C_out

    // Create C_in
    net_carry[0] = make_net("c_in", Signal::UNKNOWN);

    // Create bit nets
    for (int i = 0; i < num_bits; ++i) {
        net_a[i] = make_net("a_" + std::to_string(i), Signal::UNKNOWN);
        net_b[i] = make_net("b_" + std::to_string(i), Signal::UNKNOWN);
        net_sum[i] = make_net("sum_" + std::to_string(i), Signal::UNKNOWN);
        net_carry[i + 1] = make_net("c_" + std::to_string(i + 1), Signal::UNKNOWN);
    }

    // Build the Full Adder chain
    for (int i = 0; i < num_bits; ++i) {
        std::string prefix = "FA_" + std::to_string(i) + "_";
        
        // Internal wires
        Net* w_xor = make_net(prefix + "w_xor", Signal::UNKNOWN);
        Net* w_and1 = make_net(prefix + "w_and1", Signal::UNKNOWN);
        Net* w_and2 = make_net(prefix + "w_and2", Signal::UNKNOWN);

        // Sum logic: Sum = (A XOR B) XOR C_in
        add_xor(prefix + "xor1", net_a[i], net_b[i], w_xor);
        add_xor(prefix + "xor2", w_xor, net_carry[i], net_sum[i]);

        // Carry logic: C_out = (A AND B) OR ((A XOR B) AND C_in)
        add_and(prefix + "and1", net_a[i], net_b[i], w_and1);
        add_and(prefix + "and2", w_xor, net_carry[i], w_and2);
        add_or(prefix + "or1", w_and1, w_and2, net_carry[i + 1]);
    }

    // Verify correct number of gates and nets
    // 525 gates + 105 * 3 internal nets + 105 * 3 inputs/outputs + 1 carry-in = 525 gates, 631 nets
    EXPECT_EQ(engine.getGates().size(), static_cast<size_t>(num_bits * 5));

    // Build connections map
    engine.buildFanoutMap();

    // Schedule initial inputs transition to LOW to force propagation from UNKNOWN
    engine.scheduleEvent(0, net_carry[0], Signal::LOW);
    for (int i = 0; i < num_bits; ++i) {
        engine.scheduleEvent(0, net_a[i], Signal::LOW);
        engine.scheduleEvent(0, net_b[i], Signal::LOW);
    }

    // ─────────────────────────────────────────────────────
    // Run Step 1: Initial state settling (all LOW inputs)
    // ─────────────────────────────────────────────────────
    engine.run(100);

    // Verify all outputs settled to LOW
    for (int i = 0; i < num_bits; ++i) {
        EXPECT_EQ(net_sum[i]->getValue(), Signal::LOW) << "Bit " << i << " Sum mismatch at step 1";
    }
    EXPECT_EQ(net_carry[num_bits]->getValue(), Signal::LOW) << "Carry-out mismatch at step 1";

    // ─────────────────────────────────────────────────────
    // Run Step 2: Set A to all 1s (Signal::HIGH)
    // ─────────────────────────────────────────────────────
    // This should result in Sum = all 1s, Carry-out = 0
    for (int i = 0; i < num_bits; ++i) {
        engine.scheduleEvent(150, net_a[i], Signal::HIGH);
    }
    // Allow ample propagation time (each stage is 2 gate levels delay, max depth 105 * 2 = 210 gate levels)
    // 210 gate levels * 2ns = 420ns propagation settling time.
    engine.run(150 + 600);

    for (int i = 0; i < num_bits; ++i) {
        EXPECT_EQ(net_sum[i]->getValue(), Signal::HIGH) << "Bit " << i << " Sum mismatch at step 2";
    }
    EXPECT_EQ(net_carry[num_bits]->getValue(), Signal::LOW) << "Carry-out mismatch at step 2";

    // ─────────────────────────────────────────────────────
    // Run Step 3: Toggle C_in to 1 (Signal::HIGH)
    // ─────────────────────────────────────────────────────
    // Adding 1 to a number with all 1s (i.e. 1111...111 + 1)
    // should result in Sum = all 0s, and Carry-out = 1!
    // This triggers a full cascade propagation from C_in through all 105 bits.
    engine.scheduleEvent(800, net_carry[0], Signal::HIGH);
    engine.run(800 + 600);

    for (int i = 0; i < num_bits; ++i) {
        EXPECT_EQ(net_sum[i]->getValue(), Signal::LOW) << "Bit " << i << " Sum mismatch at step 3";
    }
    EXPECT_EQ(net_carry[num_bits]->getValue(), Signal::HIGH) << "Carry-out mismatch at step 3";
}
