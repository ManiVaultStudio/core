// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

// The file to be tested:
#include <PointData.h>

// GoogleTest header file:
#include <gtest/gtest.h>

#include <numeric>
#include <vector>


GTEST_TEST(PointData, hasZeroPointsByDefault)
{
    ASSERT_EQ(PointData{}.getNumPoints(), 0);
}


GTEST_TEST(PointData, hasOneDimensionByDefault)
{
    ASSERT_EQ(PointData{}.getNumDimensions(), 1);
}

GTEST_TEST(PointData, supportsRectangularTiledReadsAndBoundaryTiles)
{
    PointData points{};
    const std::vector<float> values{
        0, 1, 2, 3,
        10, 11, 12, 13,
        20, 21, 22, 23,
        30, 31, 32, 33,
        40, 41, 42, 43
    };
    points.setData(values, 4);
    points.makeTiled(2, 3);

    ASSERT_TRUE(points.isTiled());
    ASSERT_EQ(points.getValueAt(0), 0.0f);
    ASSERT_EQ(points.getValueAt(7), 13.0f);
    ASSERT_EQ(points.getValueAt(19), 43.0f);

    const auto boundaryTile = points.acquireTile(2, 1);
    ASSERT_TRUE(boundaryTile);
    ASSERT_EQ(boundaryTile.rows(), 1U);
    ASSERT_EQ(boundaryTile.columns(), 1U);
    ASSERT_EQ(*static_cast<const float*>(boundaryTile.data()), 43.0f);
}

GTEST_TEST(PointData, tileCacheEvictionPreservesActiveViews)
{
    PointData points{};
    std::vector<float> values(24);
    std::iota(values.begin(), values.end(), 0.0f);
    points.setData(values, 4);
    points.makeTiled(2, 2);
    points.setTiledCacheCapacity(1);

    const auto first = points.acquireTile(0, 0);
    const auto second = points.acquireTile(0, 1);
    ASSERT_EQ(points.getTiledCacheSize(), 1U);
    ASSERT_EQ(*static_cast<const float*>(first.data()), 0.0f);
    ASSERT_EQ(*static_cast<const float*>(second.data()), 2.0f);
}

GTEST_TEST(PointData, legacyRawPointerAccessMaterializesTiledData)
{
    PointData points{};
    const std::vector<float> values{ 1, 2, 3, 4, 5, 6 };
    points.setData(values, 3);
    points.makeTiled(1, 2);

    const auto* data = static_cast<const float*>(points.getDataConstVoidPtr());
    ASSERT_FALSE(points.isTiled());
    ASSERT_EQ(std::vector<float>(data, data + values.size()), values);
}

GTEST_TEST(PointData, accessVisitorsSupportRowsColumnsAndRanges)
{
    PointData points{};
    points.setData(
        std::vector<float>{
            0, 1, 2, 3,
            10, 11, 12, 13,
            20, 21, 22, 23
        },
        4);
    points.makeTiled(2, 2);

    const auto row = points.getRow(1);
    ASSERT_TRUE(row);
    ASSERT_EQ(row.columns, 4U);
    ASSERT_EQ(static_cast<const float*>(row.data)[3], 13.0f);

    std::vector<float> rangeValues;
    points.visitRange(1, 2, 1, 2, [&rangeValues](const PointDataAccessView& view) {
        const auto* values = static_cast<const float*>(view.data);

        for (std::uint64_t column = 0; column < view.columns; ++column)
            rangeValues.push_back(values[column]);
    });
    ASSERT_EQ((std::vector<float>{ 11, 12, 21, 22 }), rangeValues);

    std::size_t columnSegments = 0;
    points.visitColumns(2, 1, [&columnSegments](const PointDataAccessView& view) {
        ++columnSegments;
        ASSERT_EQ(view.columns, 1U);
    });
    ASSERT_EQ(columnSegments, 3U);

    std::vector<float> selectedColumnValues;
    points.visitColumns(
        1,
        2,
        std::vector<std::uint64_t>{ 2, 0 },
        [&selectedColumnValues](const PointDataAccessView& view) {
        const auto* values = static_cast<const float*>(view.data);

        for (std::uint64_t column = 0; column < view.columns; ++column)
            selectedColumnValues.push_back(values[column]);
        });
    ASSERT_EQ((std::vector<float>{ 21, 22, 1, 2 }), selectedColumnValues);
}
