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
    Eager,
    Tiled,
    Mixed,
    Unknown
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
using StorageBackendProgressCallback = std::function<void(float)>;

/** Base type for backend-specific conversion options. */
struct StorageBackendConversionOptions
{
    virtual ~StorageBackendConversionOptions() = default;
};

using StorageBackendConversionOptionsPtr = std::shared_ptr<const StorageBackendConversionOptions>;

/** Configuration used when converting dense point data to tiled storage. */
struct StorageBackendConfiguration : StorageBackendConversionOptions
{
    std::size_t rowTileSize    = 256;
    std::size_t columnTileSize = 8;
    std::size_t tileSizeBytes  = 1024 * 1024;
    std::size_t cacheCapacity  = 8;
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
