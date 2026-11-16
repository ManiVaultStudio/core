// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "TiledStorageGeometry.h"

#include <cstddef>
#include <cstdint>

namespace mv::pointdata::storage
{

/** Bitmask describing how rows or columns are expected to be accessed. */
enum class StorageAxisAccess
{
    None       = 0,
    Contiguous = 1,
    Random     = 2
};

/** Describes how often an access pattern is expected to be used. */
enum class StorageAccessFrequency
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

/**
 * @brief Contains a preliminary tiled-storage recommendation for a point dataset.
 *
 * The recommendation is a starting point for the storage wizard rather than
 * a guarantee of optimal performance. It describes the tile geometry, the
 * number of decoded tile payloads retained by the cache, and the optional
 * grouping of independently encoded tiles into super tiles. Callers should
 * validate the result against representative workloads and available memory.
 *
 * @author Thomas Kroes
 */
struct TiledStorageRecommendation
{
    TiledStorageGeometry geometry; /**< Recommended maximum rows and columns per tile. */
    std::size_t cacheCapacity = 1; /**< Recommended maximum number of decoded tile payloads retained in memory. */
    std::size_t superTileRows = 3; /**< Recommended number of base tile rows per packed file. */
    std::size_t superTileColumns = 3; /**< Recommended number of base tile columns per packed file. */
};

/**
 * @brief Suggests tiled-storage settings from dataset shape and intended use.
 *
 * The heuristic weighs row, column, and point access patterns, their expected
 * frequency, and the requested memory preference. It does not inspect the
 * filesystem, available RAM, or benchmark results, and it does not allocate
 * storage. Access flags may combine random and contiguous patterns; disabled
 * axes contribute no weight. The returned cache capacity is always at least
 * one and should be treated as an initial value for later measurement.
 *
 * @param rows Logical number of dataset rows.
 * @param columns Logical number of dataset columns.
 * @param elementSize Size of one stored element in bytes.
 * @param rowAccess Expected row access patterns.
 * @param rowFrequency Expected frequency of row access.
 * @param columnAccess Expected column access patterns.
 * @param columnFrequency Expected frequency of column access.
 * @param pointAccessEnabled Whether individual point access is part of the workload.
 * @param pointFrequency Expected frequency of individual point access.
 * @param memoryPreference Whether to favor smaller tiles and lower memory use.
 * @return Preliminary tiled-storage geometry, cache capacity, and packing dimensions.
 */
inline TiledStorageRecommendation recommendTiledStorage(const std::uint64_t rows, const std::uint64_t columns, const std::size_t elementSize, const StorageAxisAccess rowAccess, const StorageAccessFrequency rowFrequency, const StorageAxisAccess columnAccess, const StorageAccessFrequency columnFrequency, const bool pointAccessEnabled, const StorageAccessFrequency pointFrequency, const StorageMemoryPreference memoryPreference)
{
    const auto rowContiguousWeight    = (static_cast<int>(rowAccess) & static_cast<int>(StorageAxisAccess::Contiguous)) ? (rowFrequency == StorageAccessFrequency::Frequent ? 6 : 3) : 0;
    const auto rowRandomWeight        = (static_cast<int>(rowAccess) & static_cast<int>(StorageAxisAccess::Random)) ? (rowFrequency == StorageAccessFrequency::Frequent ? 2 : 1) : 0;
    const auto columnContiguousWeight = (static_cast<int>(columnAccess) & static_cast<int>(StorageAxisAccess::Contiguous)) ? (columnFrequency == StorageAccessFrequency::Frequent ? 6 : 3) : 0;
    const auto columnRandomWeight     = (static_cast<int>(columnAccess) & static_cast<int>(StorageAxisAccess::Random)) ? (columnFrequency == StorageAccessFrequency::Frequent ? 2 : 1) : 0;
    const auto totalWeight            = rowContiguousWeight + rowRandomWeight + columnContiguousWeight + columnRandomWeight;
    const auto axisPreference         = totalWeight == 0 ? 50 : (100 * rowContiguousWeight + 65 * rowRandomWeight + 35 * columnRandomWeight) / totalWeight;
    const auto frequentAccess           = rowFrequency == StorageAccessFrequency::Frequent || columnFrequency == StorageAccessFrequency::Frequent || (pointAccessEnabled && pointFrequency == StorageAccessFrequency::Frequent);
    const auto frequentRandomAccess     = (rowFrequency == StorageAccessFrequency::Frequent && (static_cast<int>(rowAccess) & static_cast<int>(StorageAxisAccess::Random))) || (columnFrequency == StorageAccessFrequency::Frequent && (static_cast<int>(columnAccess) & static_cast<int>(StorageAxisAccess::Random))) || (pointAccessEnabled && pointFrequency == StorageAccessFrequency::Frequent);
    const auto frequentContiguousAccess = (rowFrequency == StorageAccessFrequency::Frequent && (static_cast<int>(rowAccess) & static_cast<int>(StorageAxisAccess::Contiguous))) || (columnFrequency == StorageAccessFrequency::Frequent && (static_cast<int>(columnAccess) & static_cast<int>(StorageAxisAccess::Contiguous)));
    const auto tileBudgetBytes          = memoryPreference == StorageMemoryPreference::Low ? std::size_t { 1 } * 1024U * 1024U : frequentRandomAccess && frequentContiguousAccess ? std::size_t { 2 } * 1024U * 1024U : frequentRandomAccess ? std::size_t { 1 } * 1024U * 1024U : frequentAccess ? std::size_t { 2 } * 1024U * 1024U : std::size_t { 4 } * 1024U * 1024U;
    const auto geometry                = calculateTiledStorageGeometry(rows, columns, elementSize, tileBudgetBytes, axisPreference);
    const auto baseCacheCapacity       = calculateTiledStorageCacheCapacity(rows, columns, geometry);
    const auto cacheCapacity            = frequentAccess ? std::min<std::size_t>(64, baseCacheCapacity + 16) : baseCacheCapacity;

    return {
        geometry,
        cacheCapacity,
        3,
        3
    };
}

}
