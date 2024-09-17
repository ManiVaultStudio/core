
#include <NumericalAction.h>

#include <gtest/gtest.h>

using namespace mv::gui;

// Simple test example
TEST(NumericalAction, CheckInitialization) {
    NumericalAction numericalAction(nullptr, "", 0, 100, 50);

    EXPECT_EQ(1 + 1, 2);
}
