
#include "IntegralAction.h"

#include <gtest/gtest.h>

using namespace mv::gui;

TEST(IntegralAction, CheckInitialization) {
    IntegralAction integralAction(nullptr, "", 0, 100, 50);

    EXPECT_EQ(integralAction.getMinimum(), 0);
    EXPECT_EQ(integralAction.getMaximum(), 100);
    EXPECT_EQ(integralAction.getValue(), 50);
    EXPECT_EQ(integralAction.getPrefix(), "");
    EXPECT_EQ(integralAction.getSuffix(), "");
}
