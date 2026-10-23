#pragma once

#include "PointDataTile.h"
#include "PointDataTileCache.h"

#include <QByteArray>
#include <QStringList>

#include <cstdint>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_set>
#include <vector>

namespace mv::pointdata::tiled
{
/**
 * @brief Read-only, rectangular tiled storage for dense point data.
 *
 * Tile descriptors remain resident while tile bytes are loaded through the
 * loader on demand and retained in a bounded LRU cache. TileView instances
 * keep active payloads alive after cache eviction.
 *
 * The class exists to make access cost proportional to the requested region
 * rather than to the complete dataset. Rectangular tiles are supported so
 * callers can choose a layout appropriate for row-oriented inspection,
 * column-oriented analysis, or mixed range access. The implementation is
 * intentionally read-only at this stage; write-back and dirty-tile tracking
 * can be added without changing the descriptor and lease model. Importers may
 * attach externally owned packed supertile files so tiles can be read lazily
 * and copied directly during project serialization. A caller may provide an
 * opaque lifetime owner for externally staged files; the owner is retained by
 * the source loader until that loader is replaced or destroyed.
 *
 * @author Thomas Kroes
 */
class PointDataTiledStorage
{
public:
    /** Loads serialized bytes for a descriptor when a tile misses the cache. */
    using TileLoader = std::function<QByteArray(const TileDescriptor&)>;

    /** Generates row-major bytes for a descriptor on first tile access. */
    using TileGenerator = std::function<QByteArray(const TileDescriptor&)>;

    /** Reports conversion or generation progress in the range [0, 1]. */
    using ProgressCallback = std::function<void(float)>;

    struct MemoryEstimate
    {
        std::uint64_t rawDataBytes      = 0; /**< Logical dense matrix size in bytes. */
        std::uint64_t largestTileBytes  = 0; /**< Payload size of the largest possible tile in bytes. */
        std::uint64_t cachedTileBytes   = 0; /**< Conservative payload size for the configured cache. */
        std::uint64_t tileCount         = 0; /**< Number of row/column tiles in the resident index. */
        std::uint64_t indexBytes        = 0; /**< Conservative resident descriptor/index size in bytes. */
    };

    /**
     * Creates tiled storage with optional lazy tile loading and a resident tile index.
     *
     * @param rows Logical matrix row count.
     * @param columns Logical matrix column count.
     * @param elementSize Size of one matrix element in bytes.
     * @param rowTileSize Maximum number of rows in a tile.
     * @param columnTileSize Maximum number of columns in a tile.
     * @param loader Callback used to obtain bytes for a descriptor on a cache miss.
     * @param progress Optional progress callback for constructing the resident descriptor index.
     *
     * The loader must return exactly rows * columns * elementSize bytes for
     * the requested descriptor. An empty loader is valid when descriptors are
     * populated with resident tile bytes.
     */
    PointDataTiledStorage(std::uint64_t rows, std::uint64_t columns, std::size_t elementSize, std::size_t rowTileSize, std::size_t columnTileSize, TileLoader loader = {}, ProgressCallback progress = {});

    /** Allows independent tile-loader callbacks to run concurrently. */
    void setLoaderThreadSafe(bool threadSafe);

    /**
     * Creates tiled storage by copying a dense row-major matrix into resident tile payloads.
     *
     * The source buffer must contain rows * columns * elementSize bytes and
     * remain valid for the duration of the call. The optional callback is
     * invoked with progress values in [0, 1].
     */
    static std::shared_ptr<PointDataTiledStorage> fromDense(const char* data, std::uint64_t rows, std::uint64_t columns, std::size_t elementSize, std::size_t rowTileSize, std::size_t columnTileSize, ProgressCallback progress = {});

    /**
     * Creates read-only tiled storage whose tiles are generated only when accessed.
     *
     * The generator must return row-major bytes matching the descriptor's
     * extent and element size. It is invoked on cache misses and may therefore
     * be called again after eviction.
     * The optional progress callback covers descriptor-index construction only;
     * tile payloads remain lazy and are not generated during this call.
     */
    static std::shared_ptr<PointDataTiledStorage> fromGenerator(std::uint64_t rows, std::uint64_t columns, std::size_t elementSize, std::size_t rowTileSize, std::size_t columnTileSize, TileGenerator generator, ProgressCallback progress = {});

    /**
     * Creates read-only tiled storage whose payloads are independently encoded in one pack file.
     *
     * The block list must be in the same row-major tile order as the resident
     * descriptor index. Each tile read opens and decodes only its recorded
     * byte range.
     */
    static std::shared_ptr<PointDataTiledStorage> fromPackedFile(std::uint64_t rows, std::uint64_t columns, std::size_t elementSize, std::size_t rowTileSize, std::size_t columnTileSize, const QString& filePath, const mv::util::SharedCodec& codec, const std::vector<mv::util::PackedBlobBlock>& blocks);

    /** Creates a new lazy tile layout backed by the existing tiled storage without copying all bytes immediately. */
    static std::shared_ptr<PointDataTiledStorage> retile(const std::shared_ptr<const PointDataTiledStorage>& source, std::size_t rowTileSize, std::size_t columnTileSize, ProgressCallback progress = {});

    /** Returns the number of rows in the matrix. */
    std::uint64_t rows() const { return _rows; }

    /** Returns the number of columns in the matrix. */
    std::uint64_t columns() const { return _columns; }

    /** Returns the size of one matrix element in bytes. */
    std::size_t elementSize() const { return _elementSize; }

    /** Returns the configured number of rows per tile. */
    std::size_t rowTileSize() const { return _rowTileSize; }

    /** Returns the configured number of columns per tile. */
    std::size_t columnTileSize() const { return _columnTileSize; }

    /** Returns the number of tile rows, including a possible boundary tile. */
    std::uint64_t tileRowCount() const;

    /** Returns the number of tile columns, including a possible boundary tile. */
    std::uint64_t tileColumnCount() const;

    /**
     * Loads or retrieves a tile and returns a lifetime-safe read-only view.
     *
     * Invalid tile coordinates return an empty view. A cache eviction removes
     * only the cache entry; a view already returned remains valid through its
     * shared payload lease.
     */
    TileView acquire(std::uint64_t tileRow, std::uint64_t tileColumn) const;

    /** Reads one row-major element and converts it to float using \p typeIndex. */
    float valueAt(std::uint64_t flatIndex, std::uint32_t typeIndex) const;

    /**
     * Materializes the complete tiled matrix into a caller-owned row-major destination buffer.
     * The destination must hold rows * columns * elementSize bytes. The optional
     * callback reports progress as tiles are copied, starting at 0 and reaching
     * 1 after all tiles have been copied (also for an empty matrix). Loading a
     * tile or invoking the callback may throw.
     */
    void materialize(char* destination, ProgressCallback progress = {}) const;

    /** Returns the mutable tile descriptor index used by serialization and lazy blob assignment. */
    std::vector<TileDescriptor>& descriptors() { return _descriptors; }

    /** Returns the read-only tile descriptor index. */
    const std::vector<TileDescriptor>& descriptors() const { return _descriptors; }

    /** Sets the maximum number of tile payloads retained in the bounded LRU cache. */
    void setCacheCapacity(std::size_t capacity) const;

    /** Sets the maximum decoded payload bytes retained in the bounded LRU cache. */
    void setCacheCapacityBytes(std::size_t capacityBytes) const;

    /** Clears cached tile payloads and resets cache statistics without changing capacity. */
    void resetCache() const;

    /** Returns the configured cache capacity. */
    std::size_t cacheCapacity() const;

    /** Returns the configured decoded-byte cache capacity. */
    std::size_t cacheCapacityBytes() const;

    /** Sets whether serialized tiles are grouped into jointly encoded super tiles. */
    void setPackedTiles(bool enabled, std::size_t superTileRows = 1, std::size_t superTileColumns = 1);

    /** Returns whether serialized tiles are grouped into super tiles. */
    bool packedTiles() const { return _packedTiles; }

    /** Returns the number of base tile rows in one serialized super tile. */
    std::size_t superTileRows() const { return _superTileRows; }

    /** Returns the number of base tile columns in one serialized super tile. */
    std::size_t superTileColumns() const { return _superTileColumns; }

    /**
     * @brief Attaches already encoded packed supertile files as the tile source.
     *
     * The storage remains read-only and retains file-backed readers for lazy
     * tile access. The caller must keep the files available while tiles may be
     * requested and until project serialization copies them. When supplied,
     * \p sourceLifetime is copied into the source loader and released when
     * that loader is replaced or destroyed. The storage does not inspect or
     * otherwise manage the pointed-to object.
     *
     * @param filePaths Packed supertile files in super-tile index order.
     * @param codecName Registered codec used for the packed blocks.
     * @param blocks Packed block metadata, one entry per logical tile descriptor.
     * @param superTileIndices Super-tile index for each logical tile descriptor.
     * @param sourceLifetime Optional lifetime token for the source files. A
     * copy is retained by the source loader; the caller remains responsible
     * for making the files available until that loader is released.
     * @throws std::logic_error If packed tiled storage is not enabled.
     * @throws std::invalid_argument If paths, codec, or metadata sizes are invalid.
     * @throws std::out_of_range If a tile refers to a nonexistent supertile.
     */
    void setPackedTileSource(const QStringList& filePaths, const QString& codecName, const std::vector<mv::util::PackedBlobBlock>& blocks, const std::vector<std::size_t>& superTileIndices, std::shared_ptr<void> sourceLifetime = {});

    /** @return Whether pre-encoded supertiles are attached for lazy reads and direct serialization. */
    bool hasPackedTileSource() const { return !_packedTileSourcePaths.isEmpty(); }

    /** @return The externally owned packed supertile paths in super-tile index order. */
    const QStringList& packedTileSourcePaths() const { return _packedTileSourcePaths; }

    /** @return The registered codec name used for the attached packed supertiles. */
    const QString& packedTileSourceCodec() const { return _packedTileSourceCodec; }

    /** Returns the number of currently cached tile payloads. */
    std::size_t cacheSize() const;

    /** Returns the decoded payload bytes currently retained by the cache. */
    std::size_t cacheSizeInBytes() const;

    /** Returns the number of cache hits. */
    std::uint64_t cacheHits() const;

    /** Returns the number of cache misses. */
    std::uint64_t cacheMisses() const;

    /** Returns the number of cache evictions. */
    std::uint64_t cacheEvictions() const;

    /** Estimates logical data, resident tile, cache, and descriptor/index memory usage. */
    MemoryEstimate memoryEstimate() const;

    /** Estimates memory usage without constructing a storage instance. */
    static MemoryEstimate estimateMemory(std::uint64_t rows, std::uint64_t columns, std::size_t elementSize, std::size_t rowTileSize, std::size_t columnTileSize, std::size_t cacheCapacity, std::size_t cacheCapacityBytes = mv::pointdata::storage::defaultTileCacheCapacityBytes);

private:
    QByteArray load(const TileDescriptor& descriptor) const;
    std::uint64_t                _rows;           /**< Logical matrix row count. */
    std::uint64_t                _columns;        /**< Logical matrix column count. */
    std::size_t                  _elementSize;    /**< Size of one matrix element in bytes. */
    std::size_t                  _rowTileSize;    /**< Configured maximum rows per tile. */
    std::size_t                  _columnTileSize; /**< Configured maximum columns per tile. */
    bool                         _packedTiles = false; /**< Whether serialized tiles are grouped into super tiles. */
    std::size_t                  _superTileRows = 1; /**< Number of base tile rows in one serialized super tile. */
    std::size_t                  _superTileColumns = 1; /**< Number of base tile columns in one serialized super tile. */
    QStringList                  _packedTileSourcePaths; /**< Non-owning paths for pre-encoded staged supertiles. */
    QString                      _packedTileSourceCodec; /**< Registered codec name used by pre-encoded staged supertiles. */
    std::vector<TileDescriptor>  _descriptors;    /**< Resident row/column tile index. */
    TileLoader                   _loader;         /**< Optional callback that decodes or generates tile bytes. */
    bool                         _loaderThreadSafe = false; /**< Whether the loader can run concurrently on multiple threads. */
    mutable std::mutex                     _mutex;         /**< Protects cache state and in-flight tile loads. */
    mutable std::recursive_mutex           _loaderMutex;   /**< Serializes arbitrary loader callbacks. */
    mutable std::condition_variable        _loadCondition; /**< Wakes callers waiting for an in-flight tile load. */
    mutable std::unordered_set<std::size_t> _loadingTiles;  /**< Tile indices currently being loaded outside the mutex. */
    mutable PointDataTileCache              _cache;        /**< Bounded cache of loaded tile payloads. */
};
}
