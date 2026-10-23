#include "PointDataTiledStorage.h"

#include <biovault_bfloat16/biovault_bfloat16.h>
#include <util/CodecRegistry.h>

#include <QString>

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

PointDataTiledStorage::PointDataTiledStorage(const std::uint64_t rows, const std::uint64_t columns, const std::size_t elementSize, const std::size_t rowTileSize, const std::size_t columnTileSize, TileLoader loader, ProgressCallback progress) :
    _rows(rows),
    _columns(columns),
    _elementSize(elementSize),
    _rowTileSize(rowTileSize),
    _columnTileSize(columnTileSize),
    _loader(std::move(loader))
{
    if (rowTileSize == 0 || columnTileSize == 0 || elementSize == 0)
        throw std::invalid_argument("Invalid tiled point-data geometry");

    const auto totalTiles       = tileRowCount() * tileColumnCount();
    const auto progressInterval = std::max<std::uint64_t>(1, totalTiles / 100);
    std::uint64_t tileIndex     = 0;

    if (progress)
        progress(0.0f);

    for (std::uint64_t row = 0; row < rows; row += rowTileSize)
    {
        for (std::uint64_t column = 0; column < columns; column += columnTileSize)
        {
            _descriptors.push_back({ row, column, std::min<std::uint64_t>(rowTileSize, rows - row), std::min<std::uint64_t>(columnTileSize, columns - column), {}, {}, {} });
            ++tileIndex;

            if (progress && (tileIndex == totalTiles || tileIndex % progressInterval == 0))
                progress(static_cast<float>(tileIndex) / static_cast<float>(totalTiles));
        }
    }

    if (progress && totalTiles == 0)
        progress(1.0f);
}

void PointDataTiledStorage::setLoaderThreadSafe(const bool threadSafe)
{
    _loaderThreadSafe = threadSafe;
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

std::shared_ptr<PointDataTiledStorage> PointDataTiledStorage::fromGenerator(const std::uint64_t rows, const std::uint64_t columns, const std::size_t elementSize, const std::size_t rowTileSize, const std::size_t columnTileSize, TileGenerator generator, ProgressCallback progress)
{
    if (!generator)
        throw std::invalid_argument("Tile generator is empty");

    return std::make_shared<PointDataTiledStorage>(rows, columns, elementSize, rowTileSize, columnTileSize, std::move(generator), std::move(progress));
}

std::shared_ptr<PointDataTiledStorage> PointDataTiledStorage::fromPackedFile(const std::uint64_t rows, const std::uint64_t columns, const std::size_t elementSize, const std::size_t rowTileSize, const std::size_t columnTileSize, const QString& filePath, const mv::util::SharedCodec& codec, const std::vector<mv::util::PackedBlobBlock>& blocks)
{
    auto reader = std::make_shared<mv::util::PackedBlobFileReader>(filePath, codec);
    auto storage = std::make_shared<PointDataTiledStorage>(rows, columns, elementSize, rowTileSize, columnTileSize, [reader](const TileDescriptor& descriptor) {
        return reader->read(descriptor.packedBlock);
    });

    if (blocks.size() != storage->descriptors().size())
        throw std::invalid_argument("Packed tile block count does not match tiled point-data geometry");

    for (std::size_t index = 0; index < blocks.size(); ++index)
        storage->descriptors()[index].packedBlock = blocks[index];

    return storage;
}

std::shared_ptr<PointDataTiledStorage> PointDataTiledStorage::retile(const std::shared_ptr<const PointDataTiledStorage>& source, const std::size_t rowTileSize, const std::size_t columnTileSize, ProgressCallback progress)
{
    if (!source)
        throw std::invalid_argument("Cannot retile empty point-data storage");

    auto storage = std::make_shared<PointDataTiledStorage>(source->rows(), source->columns(), source->elementSize(), rowTileSize, columnTileSize, [source](const TileDescriptor& descriptor) {
        QByteArray bytes(static_cast<qsizetype>(descriptor.rows * descriptor.columns * source->elementSize()), '\0');

        for (std::uint64_t row = 0; row < descriptor.rows; ++row)
        {
            std::uint64_t column = 0;

            while (column < descriptor.columns)
            {
                const auto sourceTile = source->acquire((descriptor.rowOffset + row) / source->rowTileSize(), (descriptor.columnOffset + column) / source->columnTileSize());
                const auto sourceRowOffset    = descriptor.rowOffset + row - sourceTile.rowOffset();
                const auto sourceColumnOffset = descriptor.columnOffset + column - sourceTile.columnOffset();
                const auto copyColumns        = std::min(descriptor.columns - column, sourceTile.columns() - sourceColumnOffset);

                std::memcpy(bytes.data() + (row * descriptor.columns + column) * source->elementSize(), static_cast<const char*>(sourceTile.data()) + (sourceRowOffset * sourceTile.columns() + sourceColumnOffset) * source->elementSize(), copyColumns * source->elementSize());
                column += copyColumns;
            }
        }

        return bytes;
    });

    if (progress)
        progress(1.0f);

    return storage;
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
    if (tileRow >= tileRowCount() || tileColumn >= tileColumnCount())
        return {};

    const auto index = static_cast<std::size_t>(tileRow * tileColumnCount() + tileColumn);
    TileDescriptor descriptor;
    std::shared_ptr<QByteArray> bytes;

    {
        std::unique_lock lock(_mutex);

        while (true)
        {
            bytes = _cache.find(index);
            if (bytes)
                break;

            if (_loadingTiles.insert(index).second)
            {
                descriptor = _descriptors.at(index);
                break;
            }

            _loadCondition.wait(lock);
        }
    }

    if (!bytes)
    {
        try
        {
			if (descriptor.residentBytes.isEmpty())
            {
				if (_loaderThreadSafe)
					bytes = std::make_shared<QByteArray>(load(descriptor));
				else
				{
					const std::lock_guard loaderLock(_loaderMutex);
					bytes = std::make_shared<QByteArray>(load(descriptor));
				}
            }
            else
                bytes = std::make_shared<QByteArray>(descriptor.residentBytes);

            const auto expectedSize = descriptor.rows * descriptor.columns * _elementSize;
            if (bytes->size() != static_cast<qsizetype>(expectedSize))
            {
                throw std::runtime_error(QString("Decoded tiled point-data payload has an unexpected size for tile (%1, %2): expected %3 bytes, got %4 bytes")
                    .arg(descriptor.rowOffset)
                    .arg(descriptor.columnOffset)
                    .arg(expectedSize)
                    .arg(bytes->size())
                    .toStdString());
            }
        }
        catch (...)
        {
            {
                const std::lock_guard lock(_mutex);
                _loadingTiles.erase(index);
            }

            _loadCondition.notify_all();
            throw;
        }

        {
            const std::lock_guard lock(_mutex);
            _cache.insert(index, bytes);
            _loadingTiles.erase(index);
        }

        _loadCondition.notify_all();
    }

    const auto& cachedDescriptor = _descriptors.at(index);

    TileView view;
    view._bytes        = bytes;
    view._rowOffset    = cachedDescriptor.rowOffset;
    view._columnOffset = cachedDescriptor.columnOffset;
    view._rows         = cachedDescriptor.rows;
    view._columns      = cachedDescriptor.columns;
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

void PointDataTiledStorage::materialize(char* destination, ProgressCallback progress) const
{
    const auto totalTiles       = _descriptors.size();
    const auto progressInterval = std::max<std::size_t>(1, totalTiles / 100);
    std::size_t tileIndex       = 0;

    if (progress)
        progress(0.0f);

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

            ++tileIndex;

            if (progress && (tileIndex == 1 || tileIndex == totalTiles || tileIndex % progressInterval == 0))
                progress(static_cast<float>(tileIndex) / static_cast<float>(totalTiles));
        }
    }

    if (progress && totalTiles == 0)
        progress(1.0f);
}

void PointDataTiledStorage::setCacheCapacity(const std::size_t capacity) const
{
    const std::lock_guard lock(_mutex);

    _cache.setCapacity(capacity);
}

void PointDataTiledStorage::setCacheCapacityBytes(const std::size_t capacityBytes) const
{
    const std::lock_guard lock(_mutex);

    _cache.setCapacityBytes(capacityBytes);
}

void PointDataTiledStorage::resetCache() const
{
    const std::lock_guard lock(_mutex);

    _cache.reset();
}

std::size_t PointDataTiledStorage::cacheCapacity() const
{
    const std::lock_guard lock(_mutex);

    return _cache.capacity();
}

std::size_t PointDataTiledStorage::cacheCapacityBytes() const
{
    const std::lock_guard lock(_mutex);

    return _cache.capacityBytes();
}

void PointDataTiledStorage::setPackedTiles(const bool enabled, const std::size_t superTileRows, const std::size_t superTileColumns)
{
    if (enabled && (superTileRows == 0 || superTileColumns == 0))
        throw std::invalid_argument("Super-tile dimensions must be positive");

    _packedTiles       = enabled;
    _superTileRows     = enabled ? superTileRows : 1;
    _superTileColumns  = enabled ? superTileColumns : 1;
}

void PointDataTiledStorage::setPackedTileSource(const QStringList& filePaths, const QString& codecName, const std::vector<mv::util::PackedBlobBlock>& blocks, const std::vector<std::size_t>& superTileIndices, std::shared_ptr<void> sourceLifetime)
{
    if (!packedTiles())
        throw std::logic_error("Packed tile source requires packed tiled storage");

    if (filePaths.isEmpty() || codecName.isEmpty() || blocks.size() != _descriptors.size() || superTileIndices.size() != _descriptors.size())
        throw std::invalid_argument("Invalid packed tile source metadata");

    const auto codec = mv::util::codecRegistry().createCodec(nullptr, codecName);
    auto readers = std::make_shared<std::vector<std::shared_ptr<mv::util::PackedBlobFileReader>>>();
    readers->reserve(filePaths.size());
    for (const auto& filePath : filePaths)
        readers->push_back(std::make_shared<mv::util::PackedBlobFileReader>(filePath, codec));

    for (std::size_t tileIndex = 0; tileIndex < _descriptors.size(); ++tileIndex)
    {
        if (superTileIndices[tileIndex] >= static_cast<std::size_t>(filePaths.size()))
            throw std::out_of_range("Packed tile source contains an invalid super-tile index");

        _descriptors[tileIndex].packedBlock = blocks[tileIndex];
        _descriptors[tileIndex].superTileIndex = superTileIndices[tileIndex];
    }

    _loader = [readers, sourceLifetime = std::move(sourceLifetime)](const TileDescriptor& descriptor) {
        if (descriptor.superTileIndex >= readers->size())
            throw std::out_of_range("Packed tile source contains an invalid super-tile index");

        return (*readers)[descriptor.superTileIndex]->read(descriptor.packedBlock);
    };
    _packedTileSourcePaths = filePaths;
    _packedTileSourceCodec = codecName;
}

std::size_t PointDataTiledStorage::cacheSize() const
{
    const std::lock_guard lock(_mutex);

    return _cache.size();
}

std::size_t PointDataTiledStorage::cacheSizeInBytes() const
{
    const std::lock_guard lock(_mutex);

    return _cache.sizeInBytes();
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

PointDataTiledStorage::MemoryEstimate PointDataTiledStorage::estimateMemory(const std::uint64_t rows, const std::uint64_t columns, const std::size_t elementSize, const std::size_t rowTileSize, const std::size_t columnTileSize, const std::size_t cacheCapacity, const std::size_t cacheCapacityBytes)
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

    estimate.cachedTileBytes  = std::min<std::uint64_t>(residentTiles * estimate.largestTileBytes, std::max<std::size_t>(1, cacheCapacityBytes));

    // This is an intentionally conservative planning estimate for the resident descriptor/index.
    estimate.indexBytes       = estimate.tileCount * static_cast<std::uint64_t>(sizeof(TileDescriptor));

    return estimate;
}

PointDataTiledStorage::MemoryEstimate PointDataTiledStorage::memoryEstimate() const
{
    const std::lock_guard lock(_mutex);
    return estimateMemory(_rows, _columns, _elementSize, _rowTileSize, _columnTileSize, _cache.capacity(), _cache.capacityBytes());
}
}
