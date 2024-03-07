// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

// The file to be tested:
#include <PointData.h>

// GoogleTest header file:
#include <gtest/gtest.h>


GTEST_TEST(PointData, hasZeroPointsByDefault)
{
    ASSERT_EQ(PointData{}.getNumPoints(), 0);
}


GTEST_TEST(PointData, hasOneDimensionByDefault)
{
    ASSERT_EQ(PointData{}.getNumDimensions(), 1);
}
