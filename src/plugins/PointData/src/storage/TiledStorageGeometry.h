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
 * axis. An asymmetry of zero favors column-oriented tiles, an asymmetry of one
 * hundred favors row-oriented tiles, and intermediate values interpolate
 * between those shape-aware configurations.
 *
 * @author T. Kroes
 */
struct TiledStorageGeometry
{
    std::size_t rowTileSize    = 1; /**< Number of matrix rows in each calculated tile. */
    std::size_t columnTileSize = 1; /**< Number of matrix columns in each calculated tile. */
};

/** Calculates rectangular tile extents from matrix shape, target bytes, and axis asymmetry. */
inline TiledStorageGeometry calculateTiledStorageGeometry(const std::uint64_t rows, const std::uint64_t columns, const std::size_t elementSize, const std::uint64_t targetTileBytes, const std::int32_t asymmetry)
{
    constexpr std::uint64_t dominantAxisTileExtent = 8;

    const auto targetTileElements = std::max<std::uint64_t>(1, targetTileBytes / std::max<std::size_t>(1, elementSize));

    const auto clampTileSize = [](const std::uint64_t extent, const std::uint64_t value) {
        return extent == 0 ? std::size_t(1) : static_cast<std::size_t>(std::min(extent, std::max<std::uint64_t>(1, value)));
    };

    const auto normalizedAsymmetry            = std::clamp(asymmetry, 0, 100);
    const auto rowDominantColumnTileSize      = clampTileSize(columns, std::min<std::uint64_t>(columns, dominantAxisTileExtent));
    const auto rowDominantRowTileSize         = clampTileSize(rows, targetTileElements / std::max<std::size_t>(1, rowDominantColumnTileSize));
    const auto columnDominantRowTileSize      = clampTileSize(rows, std::min<std::uint64_t>(rows, dominantAxisTileExtent));
    const auto columnDominantColumnTileSize   = clampTileSize(columns, targetTileElements / std::max<std::size_t>(1, columnDominantRowTileSize));
    const auto interpolation                 = static_cast<double>(normalizedAsymmetry) / 100.0;
    const auto interpolate                   = [interpolation](const std::size_t first, const std::size_t second) {
        return static_cast<std::size_t>(std::llround(std::exp(std::log(static_cast<double>(first)) * (1.0 - interpolation) + std::log(static_cast<double>(second)) * interpolation)));
    };

    return {
        clampTileSize(rows, interpolate(rowDominantRowTileSize, columnDominantRowTileSize)),
        clampTileSize(columns, interpolate(rowDominantColumnTileSize, columnDominantColumnTileSize))
    };
}

inline TiledStorageGeometry calculateTiledStorageGeometry(const std::uint64_t rows, const std::uint64_t columns, const std::int32_t asymmetry)
{
    return calculateTiledStorageGeometry(rows, columns, sizeof(float), 256 * 1024, asymmetry);
}

/** Estimates a bounded cache capacity for the supplied matrix geometry. */
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
