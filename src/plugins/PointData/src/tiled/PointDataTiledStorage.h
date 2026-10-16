#pragma once

#include "PointDataTile.h"
#include "PointDataTileCache.h"

#include <QByteArray>

#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
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
 * can be added without changing the descriptor and lease model.
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
     *
     * The loader must return exactly rows * columns * elementSize bytes for
     * the requested descriptor. An empty loader is valid when descriptors are
     * populated with resident tile bytes.
     */
    PointDataTiledStorage(std::uint64_t rows, std::uint64_t columns, std::size_t elementSize, std::size_t rowTileSize, std::size_t columnTileSize, TileLoader loader = {});

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
     */
    static std::shared_ptr<PointDataTiledStorage> fromGenerator(std::uint64_t rows, std::uint64_t columns, std::size_t elementSize, std::size_t rowTileSize, std::size_t columnTileSize, TileGenerator generator);

    /**
     * Creates read-only tiled storage whose payloads are independently encoded in one pack file.
     *
     * The block list must be in the same row-major tile order as the resident
     * descriptor index. Each tile read opens and decodes only its recorded
     * byte range.
     */
    static std::shared_ptr<PointDataTiledStorage> fromPackedFile(std::uint64_t rows, std::uint64_t columns, std::size_t elementSize, std::size_t rowTileSize, std::size_t columnTileSize, const QString& filePath, const mv::util::SharedCodec& codec, const std::vector<PackedTileBlock>& blocks);

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

    /** Reads one row-major element and converts it to float using p typeIndex. */
    float valueAt(std::uint64_t flatIndex, std::uint32_t typeIndex) const;

    /** Materializes the complete tiled matrix into a caller-owned row-major destination buffer. */
    void materialize(char* destination) const;

    /** Returns the mutable tile descriptor index used by serialization and lazy blob assignment. */
    std::vector<TileDescriptor>& descriptors() { return _descriptors; }

    /** Returns the read-only tile descriptor index. */
    const std::vector<TileDescriptor>& descriptors() const { return _descriptors; }

    /** Sets the maximum number of tile payloads retained in the bounded LRU cache. */
    void setCacheCapacity(std::size_t capacity) const;

    /** Returns the configured cache capacity. */
    std::size_t cacheCapacity() const;

    /** Returns the number of currently cached tile payloads. */
    std::size_t cacheSize() const;

    /** Returns the number of cache hits. */
    std::uint64_t cacheHits() const;

    /** Returns the number of cache misses. */
    std::uint64_t cacheMisses() const;

    /** Returns the number of cache evictions. */
    std::uint64_t cacheEvictions() const;

    /** Estimates logical data, resident tile, cache, and descriptor/index memory usage. */
    MemoryEstimate memoryEstimate() const;

    /** Estimates memory usage without constructing a storage instance. */
    static MemoryEstimate estimateMemory(std::uint64_t rows, std::uint64_t columns, std::size_t elementSize, std::size_t rowTileSize, std::size_t columnTileSize, std::size_t cacheCapacity);

private:
    QByteArray load(const TileDescriptor& descriptor) const;
    std::uint64_t                _rows;           /**< Logical matrix row count. */
    std::uint64_t                _columns;        /**< Logical matrix column count. */
    std::size_t                  _elementSize;    /**< Size of one matrix element in bytes. */
    std::size_t                  _rowTileSize;    /**< Configured maximum rows per tile. */
    std::size_t                  _columnTileSize; /**< Configured maximum columns per tile. */
    std::vector<TileDescriptor>  _descriptors;    /**< Resident row/column tile index. */
    TileLoader                   _loader;         /**< Optional callback that decodes or generates tile bytes. */
    mutable std::recursive_mutex _mutex;          /**< Serializes cache and lazy-load access. */
    mutable PointDataTileCache   _cache;          /**< Bounded cache of loaded tile payloads. */
};
}
