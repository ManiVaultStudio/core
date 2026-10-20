// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "TiledStorageGeometry.h"

#include <cstddef>
#include <cstdint>

namespace mv::pointdata::storage
{

/** Describes how rows or columns are expected to be accessed. */
enum class StorageAxisAccess
{
    None,
    Contiguous,
    Random
};

/** Describes how often individual random points are expected to be accessed. */
enum class StoragePointAccessFrequency
{
    Incidental,
    Frequent
};

/** Describes the memory preference used by the preliminary tiled-storage heuristic. */
enum class StorageMemoryPreference
{
    Balanced,
    Low
};

/** Contains a preliminary tiled-storage recommendation for a point dataset. */
struct TiledStorageRecommendation
{
    TiledStorageGeometry geometry;
    std::size_t cacheCapacity = 1;
    std::size_t superTileRows = 3;
    std::size_t superTileColumns = 3;
};

/** Suggests tiled-storage settings from dataset shape, element size, access patterns, and memory preference. */
inline TiledStorageRecommendation recommendTiledStorage(const std::uint64_t rows, const std::uint64_t columns, const std::size_t elementSize, const StorageAxisAccess rowAccess, const StorageAxisAccess columnAccess, const StoragePointAccessFrequency pointAccessFrequency, const StorageMemoryPreference memoryPreference)
{
    const auto rowWeight = rowAccess == StorageAxisAccess::Contiguous ? 3 : rowAccess == StorageAxisAccess::Random ? 1 : 0;
    const auto columnWeight = columnAccess == StorageAxisAccess::Contiguous ? 3 : columnAccess == StorageAxisAccess::Random ? 1 : 0;
    const auto totalWeight = rowWeight + columnWeight;
    const auto axisPreference = totalWeight == 0 ? 50 : (100 * rowWeight) / totalWeight;
    const auto tileBudgetBytes = memoryPreference == StorageMemoryPreference::Low ? std::size_t { 1 } * 1024U * 1024U : pointAccessFrequency == StoragePointAccessFrequency::Frequent ? std::size_t { 2 } * 1024U * 1024U : std::size_t { 4 } * 1024U * 1024U;
    const auto geometry = calculateTiledStorageGeometry(rows, columns, elementSize, tileBudgetBytes, axisPreference);

    return {
        geometry,
        calculateTiledStorageCacheCapacity(rows, columns, geometry),
        3,
        3
    };
}

}
