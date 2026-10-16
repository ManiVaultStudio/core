// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include <tiled/PointDataTiledStorage.h>

#include <gtest/gtest.h>

#include <numeric>
#include <vector>

using mv::pointdata::tiled::PointDataTiledStorage;

namespace
{
std::vector<float> makeValues(const std::uint64_t rows, const std::uint64_t columns)
{
    std::vector<float> values(rows * columns);
    std::iota(values.begin(), values.end(), 0.0f);
    return values;
}
}

GTEST_TEST(PointData, supportsRectangularTiledReadsAndBoundaryTiles)
{
    const auto values = makeValues(5, 4);
    const auto storage = PointDataTiledStorage::fromDense(reinterpret_cast<const char*>(values.data()), 5, 4, sizeof(float), 2, 3);

    ASSERT_EQ(storage->tileRowCount(), 3U);
    ASSERT_EQ(storage->tileColumnCount(), 2U);
    ASSERT_EQ(storage->descriptors().back().rows, 1U);
    ASSERT_EQ(storage->descriptors().back().columns, 1U);
    ASSERT_FLOAT_EQ(storage->valueAt(19, 0), 19.0f);

    const auto boundaryTile = storage->acquire(2, 1);
    ASSERT_TRUE(boundaryTile);
    ASSERT_EQ(boundaryTile.rows(), 1U);
    ASSERT_EQ(boundaryTile.columns(), 1U);
    ASSERT_FLOAT_EQ(*static_cast<const float*>(boundaryTile.data()), 19.0f);
}

GTEST_TEST(PointData, tileCacheEvictionPreservesActiveViews)
{
    const auto values = makeValues(6, 4);
    const auto storage = PointDataTiledStorage::fromDense(reinterpret_cast<const char*>(values.data()), 6, 4, sizeof(float), 2, 2);
    storage->setCacheCapacity(1);

    const auto first = storage->acquire(0, 0);
    const auto second = storage->acquire(0, 1);

    ASSERT_EQ(storage->cacheSize(), 1U);
    ASSERT_FLOAT_EQ(*static_cast<const float*>(first.data()), 0.0f);
    ASSERT_FLOAT_EQ(*static_cast<const float*>(second.data()), 2.0f);
    ASSERT_GE(storage->cacheEvictions(), 1U);
}

GTEST_TEST(PointData, materializesTilesInRowMajorOrder)
{
    const auto values = makeValues(5, 4);
    const auto storage = PointDataTiledStorage::fromDense(reinterpret_cast<const char*>(values.data()), 5, 4, sizeof(float), 2, 3);
    std::vector<float> materialized(values.size());

    storage->materialize(reinterpret_cast<char*>(materialized.data()));

    ASSERT_EQ(materialized, values);
}

GTEST_TEST(PointData, retilePreservesLazyDataAccess)
{
    const auto values = makeValues(5, 4);
    const auto source = PointDataTiledStorage::fromDense(reinterpret_cast<const char*>(values.data()), 5, 4, sizeof(float), 2, 3);
    const auto retiled = PointDataTiledStorage::retile(source, 3, 2);

    ASSERT_EQ(retiled->rowTileSize(), 3U);
    ASSERT_EQ(retiled->columnTileSize(), 2U);
    ASSERT_EQ(retiled->cacheSize(), 0U);
    ASSERT_FLOAT_EQ(retiled->valueAt(13, 0), 13.0f);

    std::vector<float> materialized(values.size());
    retiled->materialize(reinterpret_cast<char*>(materialized.data()));

    ASSERT_EQ(materialized, values);
}

GTEST_TEST(PointData, generatorLoadsTilesSelectively)
{
    std::size_t loadCount = 0;
    const auto storage = PointDataTiledStorage::fromGenerator(5, 4, sizeof(float), 2, 2, [&loadCount](const auto& descriptor) {
        ++loadCount;
        QByteArray bytes(static_cast<qsizetype>(descriptor.rows * descriptor.columns * sizeof(float)), '\0');
        auto* values = reinterpret_cast<float*>(bytes.data());

        for (std::uint64_t row = 0; row < descriptor.rows; ++row)
        {
            for (std::uint64_t column = 0; column < descriptor.columns; ++column)
            {
                values[row * descriptor.columns + column] = static_cast<float>((descriptor.rowOffset + row) * 4 + descriptor.columnOffset + column);
            }
        }

        return bytes;
    });

    ASSERT_EQ(loadCount, 0U);
    ASSERT_FLOAT_EQ(storage->valueAt(9, 0), 9.0f);
    ASSERT_EQ(loadCount, 1U);
    ASSERT_FLOAT_EQ(storage->valueAt(9, 0), 9.0f);
    ASSERT_EQ(loadCount, 1U);
}

GTEST_TEST(PointData, reportsTileGeometryAndMemoryEstimate)
{
    const auto estimate = PointDataTiledStorage::estimateMemory(5, 4, sizeof(float), 2, 3, 2);

    ASSERT_EQ(estimate.rawDataBytes, 5U * 4U * sizeof(float));
    ASSERT_EQ(estimate.tileCount, 6U);
    ASSERT_EQ(estimate.largestTileBytes, 2U * 3U * sizeof(float));
    ASSERT_EQ(estimate.cachedTileBytes, 2U * estimate.largestTileBytes);
    ASSERT_GT(estimate.indexBytes, 0U);
}
