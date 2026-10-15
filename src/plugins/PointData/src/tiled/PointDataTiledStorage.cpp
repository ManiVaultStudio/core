#include "PointDataTiledStorage.h"

#include <biovault_bfloat16/biovault_bfloat16.h>

#include <algorithm>
#include <cstring>
#include <stdexcept>

namespace mv::pointdata::tiled
{
namespace
{
std::size_t typeSize(const std::uint32_t typeIndex)
{
    static constexpr std::size_t sizes[] = { sizeof(float), 2, 4, 4, 2, 2, 1, 1 };

    return typeIndex < 8 ? sizes[typeIndex] : sizeof(float);
}
}

PointDataTiledStorage::PointDataTiledStorage(const std::uint64_t rows, const std::uint64_t columns, const std::size_t elementSize, const std::size_t rowTileSize, const std::size_t columnTileSize, TileLoader loader) :
    _rows(rows),
    _columns(columns),
    _elementSize(elementSize),
    _rowTileSize(rowTileSize),
    _columnTileSize(columnTileSize),
    _loader(std::move(loader))
{
    if (rowTileSize == 0 || columnTileSize == 0 || elementSize == 0)
        throw std::invalid_argument("Invalid tiled point-data geometry");

    for (std::uint64_t row = 0; row < rows; row += rowTileSize)
    {
        for (std::uint64_t column = 0; column < columns; column += columnTileSize)
        {
            _descriptors.push_back({ row, column, std::min<std::uint64_t>(rowTileSize, rows - row), std::min<std::uint64_t>(columnTileSize, columns - column), {}, {} });
        }
    }
}

std::shared_ptr<PointDataTiledStorage> PointDataTiledStorage::fromDense(const char* data, const std::uint64_t rows, const std::uint64_t columns, const std::size_t elementSize, const std::size_t rowTileSize, const std::size_t columnTileSize, ProgressCallback progress)
{
    auto storage = std::make_shared<PointDataTiledStorage>(rows, columns, elementSize, rowTileSize, columnTileSize);

    const auto totalTiles       = storage->_descriptors.size();
    const auto progressInterval = std::max<std::size_t>(1, totalTiles / 100);
    std::size_t tileIndex       = 0;

    for (auto& tile : storage->_descriptors)
    {
        tile.residentBytes.resize(static_cast<qsizetype>(tile.rows * tile.columns * elementSize));

        for (std::uint64_t row = 0; row < tile.rows; ++row)
        {
            std::memcpy(
                tile.residentBytes.data() + row * tile.columns * elementSize,
                data + ((tile.rowOffset + row) * columns + tile.columnOffset) * elementSize,
                tile.columns * elementSize);
        }

        ++tileIndex;

        if (progress && (tileIndex == 1 || tileIndex == totalTiles || tileIndex % progressInterval == 0))
            progress(static_cast<float>(tileIndex) / static_cast<float>(totalTiles));
    }

    return storage;
}

std::shared_ptr<PointDataTiledStorage> PointDataTiledStorage::fromGenerator(const std::uint64_t rows, const std::uint64_t columns, const std::size_t elementSize, const std::size_t rowTileSize, const std::size_t columnTileSize, TileGenerator generator)
{
    if (!generator)
        throw std::invalid_argument("Tile generator is empty");

    return std::make_shared<PointDataTiledStorage>(rows, columns, elementSize, rowTileSize, columnTileSize, std::move(generator));
}

std::uint64_t PointDataTiledStorage::tileRowCount() const
{
    return (_rows + _rowTileSize - 1) / _rowTileSize;
}

std::uint64_t PointDataTiledStorage::tileColumnCount() const
{
    return (_columns + _columnTileSize - 1) / _columnTileSize;
}

QByteArray PointDataTiledStorage::load(const TileDescriptor& descriptor) const
{
    if (!descriptor.residentBytes.isEmpty())
        return descriptor.residentBytes;

    if (_loader)
        return _loader(descriptor);

    return {};
}

TileView PointDataTiledStorage::acquire(const std::uint64_t tileRow, const std::uint64_t tileColumn) const
{
    const std::lock_guard lock(_mutex);
    if (tileRow >= tileRowCount() || tileColumn >= tileColumnCount())
        return {};
    const auto index = static_cast<std::size_t>(tileRow * tileColumnCount() + tileColumn);
    auto bytes = _cache.find(index);
    if (!bytes)
    {
        bytes = std::make_shared<QByteArray>(load(_descriptors.at(index)));

        const auto& descriptor = _descriptors.at(index);
        const auto expectedSize = descriptor.rows * descriptor.columns * _elementSize;
        if (bytes->size() != static_cast<qsizetype>(expectedSize))
            throw std::runtime_error("Decoded tiled point-data payload has an unexpected size");

        _cache.insert(index, bytes);
    }

    const auto& descriptor = _descriptors.at(index);

    TileView view;
    view._bytes        = bytes;
    view._rowOffset    = descriptor.rowOffset;
    view._columnOffset = descriptor.columnOffset;
    view._rows         = descriptor.rows;
    view._columns      = descriptor.columns;
    view._elementSize  = _elementSize;

    return view;
}

float PointDataTiledStorage::valueAt(const std::uint64_t flatIndex, const std::uint32_t typeIndex) const
{
    const auto row    = flatIndex / _columns;
    const auto column = flatIndex % _columns;
    const auto tile   = acquire(row / _rowTileSize, column / _columnTileSize);
    const auto offset = ((row - tile.rowOffset()) * tile.columns() + column - tile.columnOffset()) * _elementSize;
    const auto* bytes = static_cast<const char*>(tile.data()) + offset;

    switch (typeIndex)
    {
        case 0:
            return *reinterpret_cast<const float*>(bytes);

        case 1:
            return static_cast<float>(*reinterpret_cast<const biovault::bfloat16_t*>(bytes));

        case 2:
            return static_cast<float>(*reinterpret_cast<const std::int32_t*>(bytes));

        case 3:
            return static_cast<float>(*reinterpret_cast<const std::uint32_t*>(bytes));

        case 4:
            return static_cast<float>(*reinterpret_cast<const std::int16_t*>(bytes));

        case 5:
            return static_cast<float>(*reinterpret_cast<const std::uint16_t*>(bytes));

        case 6:
            return static_cast<float>(*reinterpret_cast<const std::int8_t*>(bytes));

        case 7:
            return static_cast<float>(*reinterpret_cast<const std::uint8_t*>(bytes));

        default:
            return 0.0f;
    }
}

void PointDataTiledStorage::materialize(char* destination) const
{
    for (std::uint64_t tileRow = 0; tileRow < tileRowCount(); ++tileRow)
    {
        for (std::uint64_t tileColumn = 0; tileColumn < tileColumnCount(); ++tileColumn)
        {
            const auto tile = acquire(tileRow, tileColumn);

            for (std::uint64_t row = 0; row < tile.rows(); ++row)
            {
                std::memcpy(
                    destination + ((tile.rowOffset() + row) * _columns + tile.columnOffset()) * _elementSize,
                    static_cast<const char*>(tile.data()) + row * tile.columns() * _elementSize,
                    tile.columns() * _elementSize);
            }
        }
    }
}

void PointDataTiledStorage::setCacheCapacity(const std::size_t capacity) const
{
    const std::lock_guard lock(_mutex);

    _cache.setCapacity(capacity);
}

std::size_t PointDataTiledStorage::cacheCapacity() const
{
    const std::lock_guard lock(_mutex);

    return _cache.capacity();
}

std::size_t PointDataTiledStorage::cacheSize() const
{
    const std::lock_guard lock(_mutex);

    return _cache.size();
}

std::uint64_t PointDataTiledStorage::cacheHits() const
{
    const std::lock_guard lock(_mutex);

    return _cache.hits();
}

std::uint64_t PointDataTiledStorage::cacheMisses() const
{
    const std::lock_guard lock(_mutex);

    return _cache.misses();
}

std::uint64_t PointDataTiledStorage::cacheEvictions() const
{
    const std::lock_guard lock(_mutex);

    return _cache.evictions();
}

PointDataTiledStorage::MemoryEstimate PointDataTiledStorage::estimateMemory(const std::uint64_t rows, const std::uint64_t columns, const std::size_t elementSize, const std::size_t rowTileSize, const std::size_t columnTileSize, const std::size_t cacheCapacity)
{
    MemoryEstimate estimate;

    if (elementSize == 0 || rowTileSize == 0 || columnTileSize == 0)
        return estimate;

    const auto tileRows    = (rows + rowTileSize - 1) / rowTileSize;
    const auto tileColumns = (columns + columnTileSize - 1) / columnTileSize;

    estimate.tileCount        = tileRows * tileColumns;
    estimate.rawDataBytes     = rows * columns * elementSize;
    estimate.largestTileBytes = static_cast<std::uint64_t>(rowTileSize) * columnTileSize * elementSize;

    const auto residentTiles = std::min<std::uint64_t>(estimate.tileCount, std::max<std::size_t>(1, cacheCapacity));

    estimate.cachedTileBytes  = residentTiles * estimate.largestTileBytes;

    // This is an intentionally conservative planning estimate for the resident descriptor/index.
    estimate.indexBytes       = estimate.tileCount * static_cast<std::uint64_t>(sizeof(TileDescriptor));

    return estimate;
}

PointDataTiledStorage::MemoryEstimate PointDataTiledStorage::memoryEstimate() const
{
    const std::lock_guard lock(_mutex);
    return estimateMemory(_rows, _columns, _elementSize, _rowTileSize, _columnTileSize, _cache.capacity());
}
}
