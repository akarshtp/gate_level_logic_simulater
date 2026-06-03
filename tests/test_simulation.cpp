#include <gtest/gtest.h>
#include "SimulationEngine.h"
#include "Net.h"
#include "Signal.h"
#include "gates/AndGate.h"
#include "gates/OrGate.h"
#include "gates/NotGate.h"
#include "gates/XorGate.h"
#include "gates/MuxGate.h"

TEST(SimulationTest, HalfAdderTruthTable) {
    Net net_a("A", Signal::UNKNOWN);
    Net net_b("B", Signal::UNKNOWN);
    Net net_sum("Sum", Signal::UNKNOWN);
    Net net_carry("Carry", Signal::UNKNOWN);

    XorGate xor_sum("U1_XOR", 2);
    AndGate and_carry("U2_AND", 2);

    xor_sum.addInput(&net_a);
    xor_sum.addInput(&net_b);
    xor_sum.setOutput(&net_sum);

    and_carry.addInput(&net_a);
    and_carry.addInput(&net_b);
    and_carry.setOutput(&net_carry);

    SimulationEngine engine;
    engine.addNet(&net_a);
    engine.addNet(&net_b);
    engine.addNet(&net_sum);
    engine.addNet(&net_carry);
    engine.addGate(&xor_sum);
    engine.addGate(&and_carry);
    engine.buildFanoutMap();

    // 0 + 0 = 0, Carry = 0
    engine.scheduleEvent(0, &net_a, Signal::LOW);
    engine.scheduleEvent(0, &net_b, Signal::LOW);
    engine.run(10);
    EXPECT_EQ(net_sum.getValue(), Signal::LOW);
    EXPECT_EQ(net_carry.getValue(), Signal::LOW);

    // 0 + 1 = 1, Carry = 0
    engine.scheduleEvent(20, &net_b, Signal::HIGH);
    engine.run(30);
    EXPECT_EQ(net_sum.getValue(), Signal::HIGH);
    EXPECT_EQ(net_carry.getValue(), Signal::LOW);

    // 1 + 0 = 1, Carry = 0 (simultaneous change A=HIGH, B=LOW)
    engine.scheduleEvent(40, &net_a, Signal::HIGH);
    engine.scheduleEvent(40, &net_b, Signal::LOW);
    engine.run(50);
    EXPECT_EQ(net_sum.getValue(), Signal::HIGH);
    EXPECT_EQ(net_carry.getValue(), Signal::LOW);

    // 1 + 1 = 0, Carry = 1
    engine.scheduleEvent(60, &net_b, Signal::HIGH);
    engine.run(70);
    EXPECT_EQ(net_sum.getValue(), Signal::LOW);
    EXPECT_EQ(net_carry.getValue(), Signal::HIGH);
}

TEST(SimulationTest, GateChainPropagation) {
    Net net_a("A", Signal::UNKNOWN);
    Net net_b("B", Signal::UNKNOWN);
    Net net_a_inv("A_INV", Signal::UNKNOWN);
    Net net_out("OUT", Signal::UNKNOWN);

    NotGate not_gate("U1_NOT", 2);
    OrGate  or_gate("U2_OR", 3);

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

    // A=0, B=0 -> A_INV=1 (at t=2), OUT=1 (at t=5)
    engine.scheduleEvent(0, &net_a, Signal::LOW);
    engine.scheduleEvent(0, &net_b, Signal::LOW);

    engine.run(1); // t=1: A=0, B=0, A_INV=X, OUT=X
    EXPECT_EQ(net_a_inv.getValue(), Signal::UNKNOWN);
    EXPECT_EQ(net_out.getValue(), Signal::UNKNOWN);

    engine.run(3); // t=3: A_INV=1, OUT=X (evaluated at t=2, scheduled at t=5)
    EXPECT_EQ(net_a_inv.getValue(), Signal::HIGH);
    EXPECT_EQ(net_out.getValue(), Signal::UNKNOWN);

    engine.run(6); // t=6: A_INV=1, OUT=1
    EXPECT_EQ(net_out.getValue(), Signal::HIGH);
}
