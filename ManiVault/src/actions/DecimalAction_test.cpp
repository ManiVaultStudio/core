
#include "DecimalAction.h"

#include <gtest/gtest.h>

using namespace mv::gui;

TEST(DecimalAction, CheckInitialization) {
    DecimalAction decimalAction(nullptr, "", 0.f, 100.f, 50.f);

    EXPECT_EQ(decimalAction.getMinimum(), 0.f);
    EXPECT_EQ(decimalAction.getMaximum(), 100.f);
    EXPECT_EQ(decimalAction.getValue(), 50.f);
}
