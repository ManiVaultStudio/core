// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "../interface/PointsStorageBackend.h"

#include "PointDataTiledStorage.h"

#include <workflow/WorkflowPlan.h>

#include <functional>
#include <memory>
#include <optional>
#include <QStringList>
#include <vector>

namespace mv::pointdata::storage
{

/**
 * @brief Describes generator-backed tiled storage to be created by the tiled backend.
 *
 * The generator returns row-major bytes for the requested tile. The cache
 * capacity and packed-tile codec are applied while the tiled storage is
 * constructed. The options object itself is not retained by the backend after
 * creation.
 *
 * @author T. Kroes
 */
struct TiledStorageBackendCreationOptions final : StorageBackendCreationOptions
{
    using TileGenerator = std::function<QByteArray(std::uint64_t rowOffset, std::uint64_t columnOffset, std::uint64_t rows, std::uint64_t columns)>;

    struct PackedTileSource
    {
        QStringList filePaths;                        /**< Packed files in super-tile index order. */
        QString codecName;                            /**< Registered codec used for each packed block. */
        std::vector<std::uint64_t> offsets;           /**< Encoded byte offset for each logical tile descriptor. */
        std::vector<std::uint64_t> compressedSizes;   /**< Encoded byte count for each logical tile descriptor. */
        std::vector<std::uint64_t> uncompressedSizes; /**< Decoded byte count for each logical tile descriptor. */
        std::vector<std::size_t> superTileIndices;    /**< Packed-file index containing each logical tile. */
        std::shared_ptr<void> sourceLifetime;         /**< Owner keeping externally staged source files available. */
    };

    std::uint64_t rows                 = 0;                             /**< Number of logical matrix rows. */
    std::uint64_t columns              = 0;                             /**< Number of logical matrix columns. */
    std::size_t   elementSize          = 0;                             /**< Size of one generated element in bytes. */
    std::uint32_t elementTypeIndex     = 0;                             /**< PointData element-type index used by access views. */
    std::size_t   rowTileSize          = 256;                           /**< Maximum number of rows per tile. */
    std::size_t   columnTileSize       = 8;                             /**< Maximum number of columns per tile. */
    std::size_t   cacheCapacity        = 8;                             /**< Maximum number of generated tile payloads retained in memory. */
    std::size_t   cacheCapacityBytes   = defaultTileCacheCapacityBytes; /**< Maximum decoded tile payload bytes retained in memory. */
    bool          packedTiles          = true;                          /**< Whether serialized tiles are grouped into super tiles. */
    bool          materializeGeneratedTiles = false;                    /**< Whether generated tiles should be retained immediately instead of loading lazily. */
    std::size_t   superTileRows        = 8;                             /**< Number of base tile rows in one super tile. */
    std::size_t   superTileColumns     = 8;                             /**< Number of base tile columns in one super tile. */
    QString       tileCodecName;                                        /**< Registered codec key used for packed tile payloads. */
    int           tileCompressionLevel = 3;                             /**< Compression level for codecs that support level selection. */
    std::size_t   materializationWorkerCount = 4;                       /**< Maximum workers used to materialize generated tiles. */
    std::size_t   materializationChunkTileRows = 3;                     /**< Tile-row height of one bounded materialization chunk. */
    bool          loaderThreadSafe     = false;                         /**< Whether independent tile loader calls may run concurrently. */
    TileGenerator generator;                                            /**< Callback that generates one tile on first access. */
    std::optional<PackedTileSource> packedSource;                       /**< Optional packed tile source metadata attached at creation. */
};

/**
 * @brief Storage-backend adapter for read-only rectangular tiled point data.
 *
 * The adapter exposes the existing tile index, bounded cache, and lifetime-safe
 * tile views through the common backend contract. It does not duplicate tile
 * ownership: PointDataTiledStorage remains responsible for descriptors,
 * loading, caching, and serialization.
 *
 * Multi-tile fetches schedule acquisitions on a shared pool capped at eight
 * workers. The storage loader itself runs concurrently only when marked
 * thread-safe; visitor callbacks remain serial and ordered on the caller.
 *
 * @author Thomas Kroes
 */
class POINTDATA_EXPORT TiledPointsStorageBackend final : public PointsStorageBackend
{
public:
    /** Reads a bounded row-major matrix rectangle from an alternate strip layout. */
    using RangeLoader = std::function<QByteArray(std::uint64_t firstRow, std::uint64_t rowCount, std::uint64_t firstColumn, std::uint64_t columnCount)>;
    /**
     * Creates an adapter around \p storage and retains shared ownership of it.
     *
     * The element-type index is the PointData representation used when tile
     * bytes are converted to the facade's floating-point access API.
     */
    TiledPointsStorageBackend(const std::shared_ptr<mv::pointdata::tiled::PointDataTiledStorage>& storage, std::uint32_t elementTypeIndex);

    /** Creates tiled storage from backend-owned creation options, optionally reporting descriptor-index progress. */
    static std::shared_ptr<mv::pointdata::tiled::PointDataTiledStorage> create(const TiledStorageBackendCreationOptions& options, StorageBackendProgressCallback progress = {});

    /** Returns the tiled storage owned by this backend. */
    std::shared_ptr<mv::pointdata::tiled::PointDataTiledStorage> getStorage() const;

    /** Replaces the tiled storage owned by this backend. */
    void setStorage(std::shared_ptr<mv::pointdata::tiled::PointDataTiledStorage> storage);

    /** Enables concurrent tile-loader callbacks for this tiled backend. */
    void setLoaderThreadSafe(bool threadSafe);

    /**
     * Installs an optional strip-aware reader used by range, row, and column access.
     * The callback must return exactly rowCount * columnCount * elementSize row-major bytes and support concurrent calls.
     */
    void setRangeLoader(RangeLoader loader);

    /** Rewrites the current tiled payload into project-owned blob storage. */
    void persist(StorageBackendProgressCallback progress = {});

    /** Creates a workflow that rewrites the current tiled payload into project-owned blob storage. */
    mv::workflow::UniqueWorkflowPlan persistWorkflow();

    /** Returns the tiled backend type. */
    StorageBackendType getType() const override;

    /** Returns tiled access, lazy loading, and materialization capabilities. */
    StorageBackendCapabilities getCapabilities() const override;

    /** Returns whether this backend serializes tile resources that support project-open staging. */
    [[nodiscard]] bool supportsProjectResourceStaging() const override;

    /** Creates controls for converting to or configuring the requested backend. */
    std::unique_ptr<mv::gui::StorageBackendConversionAction> createConversionAction(StorageBackendType targetType, QObject* parent) const override;
    /** Applies the tiled cache setting without changing tile geometry or backend type. */
    void applyConfiguration(const StorageBackendConfigurationOptionsPtr& options) override;
    /** Returns the number of logical rows in the tiled storage. */
    std::uint64_t getNumPoints() const override;

    /** Returns the number of logical columns in the tiled storage. */
    std::uint64_t getNumDimensions() const override;

    /** Returns the size of one stored element in bytes. */
    std::size_t getElementSize() const override;

    /** Returns the PointData element-type index used by access views. */
    std::uint32_t getElementTypeIndex() const override;

    /** Returns the dense logical payload size represented by tiled storage. */
    std::uint64_t getStorageSizeBytes() const override;

    /** Loads the requested row and returns a lifetime-safe read-only view. */
    PointDataAccessView getRow(std::uint64_t row) const override;

    /**
     * Visits selected rows in caller order while loading intersecting tiles with bounded concurrency. Visitor callbacks run serially on the calling thread.
     *
     * Duplicate row IDs are visited repeatedly. Invalid row IDs throw std::out_of_range; an invalid visitor throws std::invalid_argument.
     */
    void visitRows(const std::vector<std::uint64_t>& rowIds, const PointDataAccessVisitor& visitor) const;

    /**
     * Visits a column range for selected rows, preserving the supplied row order and loading intersecting tiles with bounded concurrency. Visitor callbacks run serially on the calling thread.
     *
     * Duplicate row IDs are visited repeatedly. Invalid bounds throw std::out_of_range; an invalid visitor throws std::invalid_argument.
     */
    void visitRange(const std::vector<std::uint64_t>& rowIds, std::uint64_t firstColumn, std::uint64_t columnCount, const PointDataAccessVisitor& visitor) const;

    /**
     * Visits selected columns in row order while loading intersecting tiles with bounded concurrency. Visitor callbacks run serially on the calling thread.
     *
     * Consecutive column IDs are grouped into contiguous views. Invalid column IDs throw std::out_of_range; an invalid visitor throws std::invalid_argument.
     */
    void visitColumns(const std::vector<std::uint64_t>& columnIds, const PointDataAccessVisitor& visitor) const;

    /**
     * Visits selected columns for the requested row IDs, preserving row and column ID order while loading intersecting tiles with bounded concurrency. Visitor callbacks run serially on the calling thread.
     *
     * Duplicate IDs are visited repeatedly. Invalid IDs throw std::out_of_range; an invalid visitor throws std::invalid_argument.
     */
    void visitColumns(const std::vector<std::uint64_t>& columnIds, const std::vector<std::uint64_t>& rowIds, const PointDataAccessVisitor& visitor) const;

    /** Visits a bounded range in row-major order, preloading tiles with bounded concurrency without materializing unrelated tiles. */
    void visitRange(std::uint64_t firstRow, std::uint64_t rowCount, std::uint64_t firstColumn, std::uint64_t columnCount, const PointDataAccessVisitor& visitor) const override;

    /** Loads one row-major element and converts it to float. */
    float getValueAt(std::uint64_t flatIndex) const override;

    /** Materializes all tiled values into the caller-owned destination buffer. */
    void materialize(char* destination, StorageBackendProgressCallback progress = {}) const override;

private:
    std::shared_ptr<mv::pointdata::tiled::PointDataTiledStorage> _storage; /**< Tiled payload and cache owner. */
    std::uint32_t                                               _elementTypeIndex = 0; /**< Stored PointData element type. */
    RangeLoader                                                 _rangeLoader; /**< Optional region reader for independently compressed row/column strips. */
};

}
