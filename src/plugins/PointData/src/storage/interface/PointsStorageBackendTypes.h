// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>

#include <QString>

namespace mv::pointdata::storage
{

/** Default upper bound for decoded tile payloads retained by a tiled cache. */
inline constexpr std::size_t defaultTileCacheCapacityBytes = 512U * 1024U * 1024U;

/** @brief Identifies a concrete physical representation used by point data. */
enum class StorageBackendType
{
    Eager,   /**< Fully materialized dense point-data vectors. */
    Tiled,   /**< Read-only rectangular tiles loaded on demand. */
    Sparse   /**< Read-only compressed sparse point data. */
};

/**
 * @brief Returns the canonical name of a storage backend type for controls and diagnostics.
 *
 * @param type Concrete storage backend type.
 * @return Exact canonical name, or an empty string for an invalid enum value.
 */
inline QString getStorageBackendTypeName(const StorageBackendType type)
{
    switch (type)
    {
        case StorageBackendType::Eager:
            return QStringLiteral("Eager");

        case StorageBackendType::Tiled:
            return QStringLiteral("Tiled");

        case StorageBackendType::Sparse:
            return QStringLiteral("Sparse");
    }

    return {};
}

/**
 * @brief Parses an exact canonical storage backend name.
 *
 * The comparison is case-sensitive and does not trim whitespace.
 * @param typeName Canonical name returned by getStorageBackendTypeName().
 * @return Matching concrete type, or std::nullopt if the name is not recognized.
 */
inline std::optional<StorageBackendType> getStorageBackendType(const QString& typeName)
{
    constexpr std::array types = { StorageBackendType::Eager, StorageBackendType::Tiled, StorageBackendType::Sparse };

    for (const auto type : types)
    {
        if (getStorageBackendTypeName(type) == typeName)
            return type;
    }

    return std::nullopt;
}

/** Describes operations supported by a storage backend. */
enum class StorageBackendCapability : std::uint32_t
{
    None           = 0,        /**< No advertised backend operations. */
    RandomAccess   = 1u << 0,  /**< Individual row-major values can be read. */
    RangeAccess    = 1u << 1,  /**< Rectangular ranges can be visited. */
    Lazy           = 1u << 2,  /**< Data may be loaded or decoded on demand. */
    Writable       = 1u << 3,  /**< The backend supports mutation without materialization. */
    Materializable = 1u << 4   /**< The backend can be converted to eager storage. */
};

using StorageBackendCapabilities = std::uint32_t;

/** Callback receiving conversion progress in the inclusive range [0, 1]. */
using StorageBackendProgressCallback = std::function<void(float)>;

/**
 * Base type for backend-specific creation and runtime configuration options
 * passed through the storage facade.
 *
 * Concrete backends own the derived option types and validate them when they
 * are applied. The shared pointer keeps the options alive while a backend
 * configuration or representation-creation operation is in progress.
 */
struct StorageBackendConfigurationOptions
{
    virtual ~StorageBackendConfigurationOptions() = default;
};

using StorageBackendConfigurationOptionsPtr = std::shared_ptr<const StorageBackendConfigurationOptions>;

/** Base type for backend-specific configuration options used to create a new representation. */
struct StorageBackendCreationOptions : StorageBackendConfigurationOptions
{
    virtual ~StorageBackendCreationOptions() = default;
};

/**
 * Configuration used to create or reconfigure the experimental tiled backend.
 *
 * Tile dimensions are physical limits in matrix rows and columns. The cache
 * capacity limits the number of decoded tile payloads retained in memory. The
 * capacity is runtime metadata and is persisted by the tiled point-data
 * serializer; changing it does not force all tiles to be loaded.
 */
struct StorageBackendConfiguration : StorageBackendConfigurationOptions
{
    std::size_t rowTileSize         = 256;                              /**< Maximum number of matrix rows in one tile. */
    std::size_t columnTileSize      = 8;                                /**< Maximum number of matrix columns in one tile. */
    std::size_t cacheCapacity       = 8;                                /**< Maximum number of tile payloads retained in memory. */
    std::size_t cacheCapacityBytes  = defaultTileCacheCapacityBytes;    /**< Maximum decoded tile payload bytes retained in memory. */
    bool        packedTiles         = false;                            /**< Whether serialized tiles are grouped into jointly encoded super tiles. */
    std::size_t superTileRows       = 1;                                /**< Number of base tile rows in one serialized super tile. */
    std::size_t superTileColumns    = 1;                                /**< Number of base tile columns in one serialized super tile. */
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
