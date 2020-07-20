// The file to be tested:
#include <PointData.h>

// GoogleTest header file:
#include <gtest/gtest.h>


GTEST_TEST(PointData, HasZeroPointsByDefault)
{
    ASSERT_EQ(PointData{}.getNumPoints(), 0);
}


GTEST_TEST(PointData, HasOneDimensionByDefault)
{
    ASSERT_EQ(PointData{}.getNumDimensions(), 1);
}
