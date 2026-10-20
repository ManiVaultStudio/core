// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "TiledStorageGeometry.h"

#include <cstddef>
#include <cstdint>

namespace mv::pointdata::storage
{

/** Describes the access profile used by the preliminary tiled-storage heuristic. */
enum class StorageAccessProfile
{
    Mixed,
    Rows,
    Columns
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
    std::size_t targetTileMiB = 1;
    std::size_t superTileRows = 3;
    std::size_t superTileColumns = 3;
    std::int32_t asymmetry = 50;
};

/** Suggests tiled-storage settings from dataset shape, element size, access profile, and memory preference. */
inline TiledStorageRecommendation recommendTiledStorage(const std::uint64_t rows, const std::uint64_t columns, const std::size_t elementSize, const StorageAccessProfile accessProfile, const StorageMemoryPreference memoryPreference)
{
    const auto asymmetry = accessProfile == StorageAccessProfile::Rows ? 100 : accessProfile == StorageAccessProfile::Columns ? 0 : 50;
    const std::size_t targetTileMiB = memoryPreference == StorageMemoryPreference::Low ? 1U : 4U;
    const auto geometry = calculateTiledStorageGeometry(rows, columns, elementSize, targetTileMiB * 1024U * 1024U, asymmetry);

    return {
        geometry,
        calculateTiledStorageCacheCapacity(rows, columns, geometry),
        targetTileMiB,
        3,
        3,
        asymmetry
    };
}

}
