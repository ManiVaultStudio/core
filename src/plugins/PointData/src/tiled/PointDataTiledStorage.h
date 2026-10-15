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
    using TileLoader = std::function<QByteArray(const TileDescriptor&)>;
    using TileGenerator = std::function<QByteArray(const TileDescriptor&)>;
    using ProgressCallback = std::function<void(float)>;

    struct MemoryEstimate
    {
        std::uint64_t rawDataBytes      = 0; /**< Logical dense matrix size. */
        std::uint64_t largestTileBytes = 0; /**< Payload size of the largest possible tile. */
        std::uint64_t cachedTileBytes  = 0; /**< Conservative payload size for the configured cache. */
        std::uint64_t tileCount         = 0; /**< Number of row/column tiles. */
        std::uint64_t indexBytes        = 0; /**< Conservative resident descriptor/index size. */
    };

    /** Creates tiled storage with optional lazy tile loading and a resident tile index. */
    PointDataTiledStorage(std::uint64_t rows, std::uint64_t columns, std::size_t elementSize, std::size_t rowTileSize, std::size_t columnTileSize, TileLoader loader = {});

    /** Creates tiled storage by copying a dense row-major matrix into resident tile payloads. */
    static std::shared_ptr<PointDataTiledStorage> fromDense(const char* data, std::uint64_t rows, std::uint64_t columns, std::size_t elementSize, std::size_t rowTileSize, std::size_t columnTileSize, ProgressCallback progress = {});

    /** Creates read-only tiled storage whose tiles are generated only when accessed. */
    static std::shared_ptr<PointDataTiledStorage> fromGenerator(std::uint64_t rows, std::uint64_t columns, std::size_t elementSize, std::size_t rowTileSize, std::size_t columnTileSize, TileGenerator generator);

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

    /** Loads or retrieves a tile and returns a lifetime-safe read-only view. */
    TileView acquire(std::uint64_t tileRow, std::uint64_t tileColumn) const;

    /** Reads one row-major element and converts it to float. */
    float valueAt(std::uint64_t flatIndex, std::uint32_t typeIndex) const;

    /** Materializes the complete tiled matrix into a row-major destination buffer. */
    void materialize(char* destination) const;

    /** Returns the mutable tile descriptor index used by serialization and lazy blob assignment. */
    std::vector<TileDescriptor>& descriptors() { return _descriptors; }

    /** Returns the read-only tile descriptor index. */
    const std::vector<TileDescriptor>& descriptors() const { return _descriptors; }

    /** Sets the maximum number of tile payloads retained in the cache. */
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
    std::uint64_t                 _rows;
    std::uint64_t                 _columns;
    std::size_t                   _elementSize;
    std::size_t                   _rowTileSize;
    std::size_t                   _columnTileSize;
    std::vector<TileDescriptor>   _descriptors;
    TileLoader                    _loader;
    mutable std::recursive_mutex _mutex;
    mutable PointDataTileCache    _cache;
};
}
