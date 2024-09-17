
#include "NumericalAction.h"

#include <gtest/gtest.h>

using namespace mv::gui;

TEST(IntegralAction, CheckInitialization) {
    NumericalAction integralAction(nullptr, "", 0, 100, 50);

    EXPECT_EQ(integralAction.getMinimum(), 0);
    EXPECT_EQ(integralAction.getMaximum(), 100);
    EXPECT_EQ(integralAction.getValue(), 50);
    EXPECT_EQ(integralAction.getPrefix(), "");
    EXPECT_EQ(integralAction.getSuffix(), "");

    NumericalAction floatAction(nullptr, "", 0.f, 100.f, 50.f);

    EXPECT_EQ(floatAction.getMinimum(), 0.f);
    EXPECT_EQ(floatAction.getMaximum(), 100.f);
    EXPECT_EQ(floatAction.getValue(), 50.f);
    EXPECT_EQ(floatAction.getPrefix(), "");
    EXPECT_EQ(floatAction.getSuffix(), "");

    NumericalAction doubleAction(nullptr, "", 0., 100., 50.);

    EXPECT_EQ(doubleAction.getMinimum(), 0.);
    EXPECT_EQ(doubleAction.getMaximum(), 100.);
    EXPECT_EQ(doubleAction.getValue(), 50.);
    EXPECT_EQ(doubleAction.getPrefix(), "");
    EXPECT_EQ(doubleAction.getSuffix(), "");
}