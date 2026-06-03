#include <gtest/gtest.h>
#include "Net.h"
#include "Signal.h"
#include "gates/DFlipFlop.h"

TEST(DFlipFlopTest, EdgeTriggering) {
    Net d("d", Signal::LOW);
    Net clk("clk", Signal::LOW);
    Net q("q", Signal::UNKNOWN);

    DFlipFlop dff("DFF", 1);
    dff.addInput(&d);
    dff.addInput(&clk);
    dff.setOutput(&q);

    // Initial state
    EXPECT_EQ(dff.getStoredState(), Signal::UNKNOWN);
    EXPECT_EQ(dff.evaluate(), Signal::UNKNOWN);

    // D = HIGH, CLK still LOW -> no change
    d.setValue(Signal::HIGH);
    EXPECT_EQ(dff.evaluate(), Signal::UNKNOWN);

    // CLK goes HIGH -> Rising Edge! Q should latch D (HIGH)
    clk.setValue(Signal::HIGH);
    EXPECT_EQ(dff.evaluate(), Signal::HIGH);
    EXPECT_EQ(dff.getStoredState(), Signal::HIGH);

    // D changes to LOW while CLK is HIGH -> no change on output
    d.setValue(Signal::LOW);
    EXPECT_EQ(dff.evaluate(), Signal::HIGH);

    // CLK goes LOW -> Falling Edge! Output remains HIGH, nothing is latched
    clk.setValue(Signal::LOW);
    EXPECT_EQ(dff.evaluate(), Signal::HIGH);

    // D is LOW, CLK goes HIGH -> Rising Edge! Q should latch D (LOW)
    clk.setValue(Signal::HIGH);
    EXPECT_EQ(dff.evaluate(), Signal::LOW);
    EXPECT_EQ(dff.getStoredState(), Signal::LOW);
}

TEST(DFlipFlopTest, InitialStateAndUnknownClk) {
    Net d("d", Signal::HIGH);
    Net clk("clk", Signal::UNKNOWN);
    Net q("q", Signal::UNKNOWN);

    DFlipFlop dff("DFF", 1);
    dff.addInput(&d);
    dff.addInput(&clk);
    dff.setOutput(&q);

    // CLK goes from UNKNOWN to LOW -> not a rising edge
    clk.setValue(Signal::LOW);
    EXPECT_EQ(dff.evaluate(), Signal::UNKNOWN);

    // CLK goes from LOW to HIGH -> Rising Edge!
    clk.setValue(Signal::HIGH);
    EXPECT_EQ(dff.evaluate(), Signal::HIGH);

    // CLK goes from HIGH to UNKNOWN -> not a rising edge
    clk.setValue(Signal::UNKNOWN);
    d.setValue(Signal::LOW);
    EXPECT_EQ(dff.evaluate(), Signal::HIGH);
}
