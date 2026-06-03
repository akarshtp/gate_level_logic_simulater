#include <gtest/gtest.h>
#include "Net.h"
#include "Signal.h"
#include "gates/AndGate.h"
#include "gates/OrGate.h"
#include "gates/NotGate.h"
#include "gates/XorGate.h"
#include "gates/MuxGate.h"

TEST(GateTest, AndGateBasic) {
    Net in0("in0", Signal::LOW);
    Net in1("in1", Signal::LOW);
    Net out("out", Signal::UNKNOWN);

    AndGate and_gate("AND", 1);
    and_gate.addInput(&in0);
    and_gate.addInput(&in1);
    and_gate.setOutput(&out);

    // 0 AND 0 = 0
    EXPECT_EQ(and_gate.evaluate(), Signal::LOW);

    // 0 AND 1 = 0
    in1.setValue(Signal::HIGH);
    EXPECT_EQ(and_gate.evaluate(), Signal::LOW);

    // 1 AND 1 = 1
    in0.setValue(Signal::HIGH);
    EXPECT_EQ(and_gate.evaluate(), Signal::HIGH);

    // X AND 1 = X
    in0.setValue(Signal::UNKNOWN);
    EXPECT_EQ(and_gate.evaluate(), Signal::UNKNOWN);

    // X AND 0 = 0 (short-circuit on LOW)
    in1.setValue(Signal::LOW);
    EXPECT_EQ(and_gate.evaluate(), Signal::LOW);
}

TEST(GateTest, OrGateBasic) {
    Net in0("in0", Signal::LOW);
    Net in1("in1", Signal::LOW);
    Net out("out", Signal::UNKNOWN);

    OrGate or_gate("OR", 1);
    or_gate.addInput(&in0);
    or_gate.addInput(&in1);
    or_gate.setOutput(&out);

    // 0 OR 0 = 0
    EXPECT_EQ(or_gate.evaluate(), Signal::LOW);

    // 0 OR 1 = 1
    in1.setValue(Signal::HIGH);
    EXPECT_EQ(or_gate.evaluate(), Signal::HIGH);

    // 1 OR 1 = 1
    in0.setValue(Signal::HIGH);
    EXPECT_EQ(or_gate.evaluate(), Signal::HIGH);

    // X OR 0 = X
    in0.setValue(Signal::UNKNOWN);
    in1.setValue(Signal::LOW);
    EXPECT_EQ(or_gate.evaluate(), Signal::UNKNOWN);

    // X OR 1 = 1 (short-circuit on HIGH)
    in1.setValue(Signal::HIGH);
    EXPECT_EQ(or_gate.evaluate(), Signal::HIGH);
}

TEST(GateTest, NotGateBasic) {
    Net in("in", Signal::LOW);
    Net out("out", Signal::UNKNOWN);

    NotGate not_gate("NOT", 1);
    not_gate.addInput(&in);
    not_gate.setOutput(&out);

    // NOT 0 = 1
    EXPECT_EQ(not_gate.evaluate(), Signal::HIGH);

    // NOT 1 = 0
    in.setValue(Signal::HIGH);
    EXPECT_EQ(not_gate.evaluate(), Signal::LOW);

    // NOT X = X
    in.setValue(Signal::UNKNOWN);
    EXPECT_EQ(not_gate.evaluate(), Signal::UNKNOWN);
}

TEST(GateTest, XorGateBasic) {
    Net in0("in0", Signal::LOW);
    Net in1("in1", Signal::LOW);
    Net out("out", Signal::UNKNOWN);

    XorGate xor_gate("XOR", 1);
    xor_gate.addInput(&in0);
    xor_gate.addInput(&in1);
    xor_gate.setOutput(&out);

    // 0 XOR 0 = 0
    EXPECT_EQ(xor_gate.evaluate(), Signal::LOW);

    // 0 XOR 1 = 1
    in1.setValue(Signal::HIGH);
    EXPECT_EQ(xor_gate.evaluate(), Signal::HIGH);

    // 1 XOR 1 = 0
    in0.setValue(Signal::HIGH);
    EXPECT_EQ(xor_gate.evaluate(), Signal::LOW);

    // X XOR 1 = X
    in0.setValue(Signal::UNKNOWN);
    EXPECT_EQ(xor_gate.evaluate(), Signal::UNKNOWN);
}

TEST(GateTest, MuxGateBasic) {
    Net d0("d0", Signal::LOW);
    Net d1("d1", Signal::HIGH);
    Net sel("sel", Signal::LOW);
    Net out("out", Signal::UNKNOWN);

    MuxGate mux("MUX", 1);
    mux.addInput(&d0);
    mux.addInput(&d1);
    mux.addInput(&sel);
    mux.setOutput(&out);

    // SEL = 0 -> output d0 (LOW)
    EXPECT_EQ(mux.evaluate(), Signal::LOW);

    // SEL = 1 -> output d1 (HIGH)
    sel.setValue(Signal::HIGH);
    EXPECT_EQ(mux.evaluate(), Signal::HIGH);

    // SEL = X, but D0 == D1 (both LOW) -> output LOW
    sel.setValue(Signal::UNKNOWN);
    d1.setValue(Signal::LOW);
    EXPECT_EQ(mux.evaluate(), Signal::LOW);

    // SEL = X, D0 != D1 -> output UNKNOWN
    d1.setValue(Signal::HIGH);
    EXPECT_EQ(mux.evaluate(), Signal::UNKNOWN);
}
