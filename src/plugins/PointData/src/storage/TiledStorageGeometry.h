// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>

namespace mv::pointdata::storage
{

/**
 * @brief Describes the automatically calculated geometry of a tiled point-data matrix.
 *
 * The geometry keeps the approximate number of elements per tile bounded while
 * allowing callers to favor contiguous access along either the row or column
 * axis. A dominance of zero favors larger row tiles, a dominance of one
 * hundred favors larger column tiles, and intermediate values interpolate
 * between those shape-aware configurations.
 *
 * @author T. Kroes
 */
struct TiledStorageGeometry
{
    std::size_t rowTileSize    = 1;
    std::size_t columnTileSize = 1;
};

inline TiledStorageGeometry calculateTiledStorageGeometry(const std::uint64_t rows, const std::uint64_t columns, const std::int32_t dominance)
{
    constexpr std::uint64_t targetTileElements      = 65536;
    constexpr std::uint64_t dominantAxisTileExtent  = 8;

    const auto clampTileSize = [](const std::uint64_t extent, const std::uint64_t value) {
        return extent == 0 ? std::size_t(1) : static_cast<std::size_t>(std::min(extent, std::max<std::uint64_t>(1, value)));
    };

    const auto normalizedDominance            = std::clamp(dominance, 0, 100);
    const auto rowDominantColumnTileSize      = clampTileSize(columns, std::min<std::uint64_t>(columns, dominantAxisTileExtent));
    const auto rowDominantRowTileSize         = clampTileSize(rows, targetTileElements / std::max<std::size_t>(1, rowDominantColumnTileSize));
    const auto columnDominantRowTileSize      = clampTileSize(rows, std::min<std::uint64_t>(rows, dominantAxisTileExtent));
    const auto columnDominantColumnTileSize   = clampTileSize(columns, targetTileElements / std::max<std::size_t>(1, columnDominantRowTileSize));
    const auto interpolation                 = static_cast<double>(normalizedDominance) / 100.0;
    const auto interpolate                   = [interpolation](const std::size_t first, const std::size_t second) {
        return static_cast<std::size_t>(std::llround(std::exp(std::log(static_cast<double>(first)) * (1.0 - interpolation) + std::log(static_cast<double>(second)) * interpolation)));
    };

    return {
        clampTileSize(rows, interpolate(rowDominantRowTileSize, columnDominantRowTileSize)),
        clampTileSize(columns, interpolate(rowDominantColumnTileSize, columnDominantColumnTileSize))
    };
}

inline std::size_t calculateTiledStorageCacheCapacity(const std::uint64_t rows, const std::uint64_t columns, const TiledStorageGeometry& geometry)
{
    const auto rowTileCount    = (rows + geometry.rowTileSize - 1) / geometry.rowTileSize;
    const auto columnTileCount = (columns + geometry.columnTileSize - 1) / geometry.columnTileSize;
    const auto tileCount       = rowTileCount * columnTileCount;
    const auto aspectRatio     = static_cast<double>(std::max(geometry.rowTileSize, geometry.columnTileSize)) / std::min(geometry.rowTileSize, geometry.columnTileSize);
    const auto directionalBias = 1.0 + std::min(1.0, std::log2(std::max(1.0, aspectRatio)) / 10.0);
    const auto capacity        = static_cast<std::uint64_t>(std::sqrt(static_cast<double>(std::max<std::uint64_t>(1, tileCount)) * directionalBias));

    return static_cast<std::size_t>(std::clamp<std::uint64_t>(capacity, 1, 64));
}

}
