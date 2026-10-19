// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>

namespace mv::pointdata::storage
{

/** Identifies the physical representation used by a concrete point dataset. */
enum class StorageBackendType
{
    Eager,   /**< Fully materialized dense point-data vectors. */
    Tiled,   /**< Read-only rectangular tiles loaded on demand. */
    Mixed,   /**< A transitional or combined representation. */
    Unknown  /**< No recognized concrete representation. */
};

/** Describes operations supported by a storage backend. */
enum class StorageBackendCapability : std::uint32_t
{
    None          = 0,
    RandomAccess  = 1u << 0,
    RangeAccess   = 1u << 1,
    Lazy          = 1u << 2,
    Writable      = 1u << 3,
    Materializable = 1u << 4
};

using StorageBackendCapabilities = std::uint32_t;

/** Callback receiving conversion progress in the inclusive range [0, 1]. */
using StorageBackendProgressCallback = std::function<void(float)>;

/**
 * Base type for backend-specific conversion and reconfiguration options passed
 * through the storage facade.
 *
 * Concrete backends own the derived option types and validate them when they
 * are applied. The shared pointer keeps the options alive while a conversion
 * or configuration operation is in progress.
 */
struct StorageBackendConversionOptions
{
    virtual ~StorageBackendConversionOptions() = default;
};

using StorageBackendConversionOptionsPtr = std::shared_ptr<const StorageBackendConversionOptions>;

/** Base type for backend-specific options used to create a new representation. */
struct StorageBackendCreationOptions : StorageBackendConversionOptions
{
    virtual ~StorageBackendCreationOptions() = default;
};

/**
 * Configuration used to create or reconfigure the experimental tiled backend.
 *
 * Tile dimensions are physical limits in matrix rows and columns. The cache
 * capacity limits the number of decoded tile payloads retained in memory; it
 * does not change the serialized dataset or force all tiles to be loaded.
 */
struct StorageBackendConfiguration : StorageBackendConversionOptions
{
    std::size_t rowTileSize    = 256;           /**< Maximum number of matrix rows in one tile. */
    std::size_t columnTileSize = 8;             /**< Maximum number of matrix columns in one tile. */
    std::size_t cacheCapacity  = 8;             /**< Maximum number of tile payloads retained in memory. */
};

constexpr StorageBackendCapabilities operator|(const StorageBackendCapability left, const StorageBackendCapability right)
{
    return static_cast<StorageBackendCapabilities>(left) | static_cast<StorageBackendCapabilities>(right);
}

constexpr StorageBackendCapabilities operator|(const StorageBackendCapabilities left, const StorageBackendCapability right)
{
    return left | static_cast<StorageBackendCapabilities>(right);
}

constexpr bool hasStorageBackendCapability(const StorageBackendCapabilities capabilities, const StorageBackendCapability capability)
{
    return (capabilities & static_cast<StorageBackendCapabilities>(capability)) != 0;
}

}
