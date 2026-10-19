// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "PointsStorageBackend.h"

#include "../tiled/PointDataTiledStorage.h"

#include <functional>
#include <memory>

namespace mv::pointdata::storage
{

/**
 * @brief Describes generator-backed tiled storage to be created by the tiled backend.
 *
 * The generator returns row-major bytes for the requested tile. The options
 * are consumed during creation and are not retained by the backend after the
 * tiled storage has been constructed.
 *
 * @author T. Kroes
 */
struct TiledStorageBackendCreationOptions final : StorageBackendCreationOptions
{
    using TileGenerator = std::function<QByteArray(std::uint64_t rowOffset, std::uint64_t columnOffset, std::uint64_t rows, std::uint64_t columns)>;

    std::uint64_t rows                 = 0;   /**< Number of logical matrix rows. */
    std::uint64_t columns              = 0;   /**< Number of logical matrix columns. */
    std::size_t   elementSize          = 0;   /**< Size of one generated element in bytes. */
    std::uint32_t elementTypeIndex     = 0;   /**< PointData element-type index used by access views. */
    std::size_t   rowTileSize          = 256; /**< Maximum number of rows per tile. */
    std::size_t   columnTileSize       = 8;   /**< Maximum number of columns per tile. */
    std::size_t   cacheCapacity        = 8;   /**< Maximum number of generated tile payloads retained in memory. */
    TileGenerator generator;                 /**< Callback that generates one tile on first access. */
};

/**
 * @brief Storage-backend adapter for read-only rectangular tiled point data.
 *
 * The adapter exposes the existing tile index, bounded cache, and lifetime-safe
 * tile views through the common backend contract. It does not duplicate tile
 * ownership: PointDataTiledStorage remains responsible for descriptors,
 * loading, caching, and serialization.
 *
 * @author Thomas Kroes
 */
class POINTDATA_EXPORT TiledPointsStorageBackend final : public PointsStorageBackend
{
public:
    /**
     * Creates an adapter around \p storage and retains shared ownership of it.
     *
     * The element-type index is the PointData representation used when tile
     * bytes are converted to the facade's floating-point access API.
     */
    TiledPointsStorageBackend(const std::shared_ptr<mv::pointdata::tiled::PointDataTiledStorage>& storage, std::uint32_t elementTypeIndex);

    /** Creates tiled storage from backend-owned creation options. */
    static std::shared_ptr<mv::pointdata::tiled::PointDataTiledStorage> create(const TiledStorageBackendCreationOptions& options);

    /** Returns the tiled storage owned by this backend. */
    std::shared_ptr<mv::pointdata::tiled::PointDataTiledStorage> getStorage() const;

    /** Replaces the tiled storage owned by this backend. */
    void setStorage(std::shared_ptr<mv::pointdata::tiled::PointDataTiledStorage> storage);

    StorageBackendType getType() const override;
    StorageBackendCapabilities getCapabilities() const override;
    std::unique_ptr<mv::gui::StorageBackendConversionAction> createConversionAction(StorageBackendType targetType, QObject* parent) const override;
    /** Applies the tiled cache setting without changing tile geometry or backend type. */
    void applyConfiguration(const StorageBackendConversionOptionsPtr& options) override;
    std::uint64_t getNumPoints() const override;
    std::uint64_t getNumDimensions() const override;
    std::size_t getElementSize() const override;
    std::uint32_t getElementTypeIndex() const override;
    PointDataAccessView getRow(std::uint64_t row) const override;
    void visitRange(std::uint64_t firstRow, std::uint64_t rowCount, std::uint64_t firstColumn, std::uint64_t columnCount, const PointDataAccessVisitor& visitor) const override;
    float getValueAt(std::uint64_t flatIndex) const override;
    void materialize(char* destination) const override;

private:
    std::shared_ptr<mv::pointdata::tiled::PointDataTiledStorage> _storage; /**< Tiled payload and cache owner. */
    std::uint32_t                                               _elementTypeIndex = 0; /**< Stored PointData element type. */
};

}
