#pragma once

#include "PointDataTile.h"
#include "PointDataTileCache.h"

#include <QVariantMap>

#include <functional>
#include <memory>
#include <mutex>
#include <vector>

namespace mv::pointdata::tiled
{
class PointDataTiledStorage
{
public:
    using TileLoader = std::function<QByteArray(const TileDescriptor&)>;

    PointDataTiledStorage(std::uint64_t rows, std::uint64_t columns, std::size_t elementSize,
        std::size_t rowTileSize, std::size_t columnTileSize, TileLoader loader = {});

    static std::shared_ptr<PointDataTiledStorage> fromDense(const char* data, std::uint64_t rows,
        std::uint64_t columns, std::size_t elementSize, std::size_t rowTileSize, std::size_t columnTileSize);

    std::uint64_t rows() const { return _rows; }
    std::uint64_t columns() const { return _columns; }
    std::size_t elementSize() const { return _elementSize; }
    std::size_t rowTileSize() const { return _rowTileSize; }
    std::size_t columnTileSize() const { return _columnTileSize; }
    std::uint64_t tileRowCount() const;
    std::uint64_t tileColumnCount() const;
    TileView acquire(std::uint64_t tileRow, std::uint64_t tileColumn) const;
    float valueAt(std::uint64_t flatIndex, std::uint32_t typeIndex) const;
    void materialize(char* destination) const;
    std::vector<TileDescriptor>& descriptors() { return _descriptors; }
    const std::vector<TileDescriptor>& descriptors() const { return _descriptors; }
    void setCacheCapacity(std::size_t capacity) const;
    std::size_t cacheCapacity() const;
    std::size_t cacheSize() const;

private:
    QByteArray load(const TileDescriptor& descriptor) const;
    std::uint64_t _rows;
    std::uint64_t _columns;
    std::size_t _elementSize;
    std::size_t _rowTileSize;
    std::size_t _columnTileSize;
    std::vector<TileDescriptor> _descriptors;
    TileLoader _loader;
    mutable std::recursive_mutex _mutex;
    mutable PointDataTileCache _cache;
};
}
