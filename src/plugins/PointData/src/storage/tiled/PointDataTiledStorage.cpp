#include "PointDataTiledStorage.h"
#include "PackedTileFile.h"

#include <biovault_bfloat16/biovault_bfloat16.h>
#include <util/CodecRegistry.h>
#include <util/Serialization.h>

#include <QDebug>
#include <QString>
#include <QDir>
#include <QTemporaryDir>
#include <QUuid>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <exception>
#include <future>
#include <limits>
#include <mutex>
#include <stdexcept>
#include <thread>

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

std::shared_ptr<PointDataTiledStorage> PointDataTiledStorage::fromGenerator(const std::uint64_t rows, const std::uint64_t columns, const std::size_t elementSize, const std::size_t rowTileSize, const std::size_t columnTileSize, TileGenerator generator, ProgressCallback progress, const bool materializeGeneratedTiles, const bool parallelMaterialization, const std::size_t maxWorkerCount, const std::size_t chunkTileRowFactor)
{
    if (!generator)
        throw std::invalid_argument("Tile generator is empty");

    auto descriptorProgress = progress;
    if (materializeGeneratedTiles && progress)
        descriptorProgress = [progress](const float value) { progress(value * 0.1f); };

    auto storage = std::make_shared<PointDataTiledStorage>(rows, columns, elementSize, rowTileSize, columnTileSize, std::move(generator), std::move(descriptorProgress));

    if (!materializeGeneratedTiles)
        return storage;

    const auto totalTiles       = storage->_descriptors.size();
    if (progress)
        progress(0.1f);

    const auto maxWorkers = parallelMaterialization ? std::clamp<std::size_t>(maxWorkerCount, 1, 256) : 1;
    const auto chunkTileRows = std::max<std::size_t>(1, chunkTileRowFactor);
    const auto tileColumnCount = storage->tileColumnCount();
    const auto chunkTileCount = tileColumnCount != 0 && chunkTileRows > std::numeric_limits<std::size_t>::max() / tileColumnCount ? totalTiles : std::max<std::size_t>(1, chunkTileRows * tileColumnCount);
    const auto chunkCount = totalTiles == 0 ? 0 : 1 + (totalTiles - 1) / chunkTileCount;
    std::atomic<std::size_t> activeWorkers{ 0 };
    std::atomic<std::size_t> maximumActiveWorkers{ 0 };
    std::atomic<std::uint64_t> workerNanoseconds{ 0 };
    std::atomic<std::size_t> completedTiles{ 0 };
    std::atomic<bool> failed{ false };
    std::mutex exceptionMutex;
    std::mutex progressWaitMutex;
    std::condition_variable progressCondition;
    std::exception_ptr failure;
    std::exception_ptr progressFailure;
    const auto conversionStart = std::chrono::steady_clock::now();
    for (std::size_t chunkStart = 0; chunkStart < totalTiles && !failed.load(std::memory_order_relaxed); chunkStart += chunkTileCount)
    {
        const auto chunkEnd = std::min(totalTiles, chunkStart + chunkTileCount);
        std::atomic<std::size_t> nextTile{ chunkStart };
        const auto workerCount = std::min<std::size_t>(chunkEnd - chunkStart, maxWorkers);
        const auto worker = [&]() {
            while (!failed.load(std::memory_order_relaxed))
            {
                const auto index = nextTile.fetch_add(1, std::memory_order_relaxed);
                if (index >= chunkEnd)
                    return;

                bool generatorActive = false;
                try
                {
                    auto& descriptor = storage->_descriptors[index];
                    if (descriptor.rows != 0 && descriptor.columns > (std::numeric_limits<std::uint64_t>::max)() / descriptor.rows)
                        throw std::overflow_error("Generated tile dimensions exceed the supported byte size");

                    const auto elementCount = descriptor.rows * descriptor.columns;
                    if (elementSize == 0 || elementCount > static_cast<std::uint64_t>((std::numeric_limits<qsizetype>::max)()) / elementSize)
                        throw std::overflow_error("Generated tile payload exceeds the supported byte size");

                    const auto active = activeWorkers.fetch_add(1, std::memory_order_relaxed) + 1;
                    generatorActive = true;
                    auto maximum = maximumActiveWorkers.load(std::memory_order_relaxed);
                    while (active > maximum && !maximumActiveWorkers.compare_exchange_weak(maximum, active, std::memory_order_relaxed))
                    {
                    }
                    const auto tileStart = std::chrono::steady_clock::now();
                    auto bytes = storage->_loader(descriptor);
                    workerNanoseconds.fetch_add(static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - tileStart).count()), std::memory_order_relaxed);
                    activeWorkers.fetch_sub(1, std::memory_order_relaxed);
                    generatorActive = false;
                    const auto expectedBytes = elementCount * elementSize;
                    if (bytes.size() != static_cast<qsizetype>(expectedBytes))
                        throw std::runtime_error("Generated tile payload size does not match its descriptor");
                    descriptor.residentBytes = std::move(bytes);

                    const auto completed = completedTiles.fetch_add(1, std::memory_order_relaxed) + 1;
                    progressCondition.notify_one();
                    if (progress && workerCount == 1)
                    {
                        progress(0.1f + static_cast<float>(completed) / static_cast<float>(totalTiles) * 0.9f);
                    }
                }
                catch (...)
                {
                    if (generatorActive)
                        activeWorkers.fetch_sub(1, std::memory_order_relaxed);
                    failed.store(true, std::memory_order_relaxed);
                    progressCondition.notify_all();
                    std::lock_guard<std::mutex> guard(exceptionMutex);
                    if (!failure)
                        failure = std::current_exception();
                    return;
                }
            }
        };

        if (workerCount == 1)
            worker();
        else
        {
            std::vector<std::jthread> workers;
            workers.reserve(workerCount);
            for (std::size_t workerIndex = 0; workerIndex < workerCount; ++workerIndex)
                workers.emplace_back(worker);
            if (progress)
            {
                auto reportedTiles = chunkStart;
                while (reportedTiles < chunkEnd && !failed.load(std::memory_order_relaxed))
                {
                    std::unique_lock<std::mutex> lock(progressWaitMutex);
                    progressCondition.wait_for(lock, std::chrono::milliseconds(20), [&] {
                        return completedTiles.load(std::memory_order_relaxed) > reportedTiles || failed.load(std::memory_order_relaxed);
                    });
                    reportedTiles = completedTiles.load(std::memory_order_relaxed);
                    lock.unlock();
                    if (reportedTiles > chunkStart)
                    {
                        try
                        {
                            progress(0.1f + static_cast<float>(reportedTiles) / static_cast<float>(totalTiles) * 0.9f);
                        }
                        catch (...)
                        {
                            progressFailure = std::current_exception();
                            failed.store(true, std::memory_order_relaxed);
                            progressCondition.notify_all();
                        }
                    }
                }
            }
            workers.clear(); // jthread destruction joins every worker.
        }
    }

    if (progressFailure)
        std::rethrow_exception(progressFailure);

    if (failure)
        std::rethrow_exception(failure);

    if (parallelMaterialization)
    {
        const auto elapsedMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - conversionStart).count();
        const auto workerMilliseconds = workerNanoseconds.load(std::memory_order_relaxed) / 1000000;
        qInfo().noquote() << QStringLiteral("[PointData] Tiled materialization: %1 tiles in %2 chunks (%3 tile rows per chunk); requested workers %4; maximum concurrent generators %5; aggregate generator time %6 ms; elapsed %7 ms")
            .arg(totalTiles)
            .arg(chunkCount)
            .arg(chunkTileRows)
            .arg(maxWorkers)
            .arg(maximumActiveWorkers.load(std::memory_order_relaxed))
            .arg(workerMilliseconds)
            .arg(elapsedMilliseconds);
    }

    storage->_loader = {};
    return storage;
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

std::shared_ptr<PointDataTiledStorage> PointDataTiledStorage::retileToPackedFiles(const std::shared_ptr<const PointDataTiledStorage>& source, const std::size_t rowTileSize, const std::size_t columnTileSize, const std::size_t superTileRows, const std::size_t superTileColumns, const mv::util::SharedCodec& codec, ProgressCallback progress, const std::size_t maxWorkerCount, const std::size_t chunkTileRowFactor, std::function<mv::util::SharedCodec()> codecFactory)
{
    if (!source)
        throw std::invalid_argument("Cannot convert empty point-data storage");
    if (rowTileSize == 0 || columnTileSize == 0 || superTileRows == 0 || superTileColumns == 0 || !codec)
        throw std::invalid_argument("Invalid packed retile configuration");

    auto storage = std::make_shared<PointDataTiledStorage>(source->rows(), source->columns(), source->elementSize(), rowTileSize, columnTileSize);
    storage->setCacheCapacity(source->cacheCapacity());
    storage->setCacheCapacityBytes(source->cacheCapacityBytes());
    storage->setPackedTiles(true, superTileRows, superTileColumns);

    const auto tileRows = storage->tileRowCount();
    const auto tileColumns = storage->tileColumnCount();
    const auto tileCount = storage->descriptors().size();
    if (progress)
        progress(0.0f);
    if (tileCount == 0)
    {
        if (progress)
            progress(1.0f);
        return storage;
    }

    auto temporaryDirectory = std::make_shared<QTemporaryDir>(QDir::temp().filePath(QStringLiteral("manivault-retile-XXXXXX")));
    if (!temporaryDirectory->isValid())
        throw std::runtime_error("Unable to create temporary directory for tiled storage conversion");

    const auto superTileColumnCount = (tileColumns + superTileColumns - 1) / superTileColumns;
    const auto totalSuperTileRows = (tileRows + superTileRows - 1) / superTileRows;
    const auto totalSuperTiles = totalSuperTileRows * superTileColumnCount;
    std::vector<mv::util::PackedBlobBlock> blocks(tileCount);
    std::vector<std::size_t> superTileIndices(tileCount);
    QStringList filePaths;
    filePaths.resize(static_cast<qsizetype>(totalSuperTiles));
    std::size_t completedTiles = 0;
    for (std::size_t superTileIndex = 0; superTileIndex < totalSuperTiles; ++superTileIndex)
    {
        const auto fileName = QUuid::createUuid().toString(QUuid::WithoutBraces) + QStringLiteral(".ptpack");
        filePaths[static_cast<qsizetype>(superTileIndex)] = QDir(temporaryDirectory->path()).filePath(fileName);
    }

    const auto maximumWorkers = codecFactory ? std::clamp<std::size_t>(maxWorkerCount, 1, 256) : 1;
    std::vector<mv::util::SharedCodec> workerCodecs;
    workerCodecs.reserve(maximumWorkers);
    for (std::size_t workerIndex = 0; workerIndex < maximumWorkers; ++workerIndex)
    {
        auto workerCodec = codecFactory ? codecFactory() : codec;
        if (!workerCodec)
            throw std::runtime_error("Unable to create a packed-tile conversion codec");
        workerCodecs.push_back(std::move(workerCodec));
    }

    const auto materializeSuperTile = [&](const std::size_t superTileIndex, const mv::util::SharedCodec& jobCodec) {
        const auto superTileRow = (superTileIndex / superTileColumnCount) * superTileRows;
        const auto superTileColumn = (superTileIndex % superTileColumnCount) * superTileColumns;
        const auto lastTileRow = std::min<std::uint64_t>(tileRows, superTileRow + superTileRows);
        const auto lastTileColumn = std::min<std::uint64_t>(tileColumns, superTileColumn + superTileColumns);
        mv::pointdata::tiled::PackedTileFileWriter writer(filePaths[static_cast<qsizetype>(superTileIndex)], jobCodec);
        std::size_t tileCountInSuperTile = 0;

        for (std::uint64_t tileRow = superTileRow; tileRow < lastTileRow; ++tileRow)
        {
            for (std::uint64_t tileColumn = superTileColumn; tileColumn < lastTileColumn; ++tileColumn)
            {
                const auto tileIndex = static_cast<std::size_t>(tileRow * tileColumns + tileColumn);
                const auto& descriptor = storage->descriptors()[tileIndex];
                if (descriptor.rows != 0 && descriptor.columns > static_cast<std::uint64_t>((std::numeric_limits<qsizetype>::max)()) / descriptor.rows / source->elementSize())
                    throw std::overflow_error("Converted tile exceeds supported byte size");

                const auto tileBytes = descriptor.rows * descriptor.columns * source->elementSize();
                QByteArray payload(static_cast<qsizetype>(tileBytes), Qt::Uninitialized);
                for (std::uint64_t localRow = 0; localRow < descriptor.rows; ++localRow)
                {
                    std::uint64_t localColumn = 0;
                    const auto sourceRow = descriptor.rowOffset + localRow;
                    while (localColumn < descriptor.columns)
                    {
                        const auto column = descriptor.columnOffset + localColumn;
                        const auto sourceTile = source->acquire(sourceRow / source->rowTileSize(), column / source->columnTileSize());
                        if (!sourceTile)
                            throw std::runtime_error("Unable to read source tile during storage conversion");

                        const auto sourceRowOffset = sourceRow - sourceTile.rowOffset();
                        const auto sourceColumnOffset = column - sourceTile.columnOffset();
                        const auto copyColumns = std::min<std::uint64_t>(descriptor.columns - localColumn, sourceTile.columns() - sourceColumnOffset);
                        const auto sourceOffset = (sourceRowOffset * sourceTile.columns() + sourceColumnOffset) * source->elementSize();
                        const auto targetOffset = (localRow * descriptor.columns + localColumn) * source->elementSize();
                        std::memcpy(payload.data() + targetOffset, static_cast<const char*>(sourceTile.data()) + sourceOffset, static_cast<std::size_t>(copyColumns * source->elementSize()));
                        localColumn += copyColumns;
                    }
                }

                blocks[tileIndex] = writer.append(payload);
                superTileIndices[tileIndex] = superTileIndex;
                ++tileCountInSuperTile;
            }
        }

        writer.close();
        return tileCountInSuperTile;
    };

    const auto chunkTileRows = std::clamp<std::size_t>(chunkTileRowFactor, 1, 256);
    const auto requestedSuperTileRowsPerChunk = 1 + (chunkTileRows - 1) / superTileRows;
    const auto superTileRowsPerChunk = std::min<std::size_t>(static_cast<std::size_t>(totalSuperTileRows), std::max(requestedSuperTileRowsPerChunk, maximumWorkers));
    const auto superTileChunkSize = superTileColumnCount != 0 && superTileRowsPerChunk > std::numeric_limits<std::size_t>::max() / superTileColumnCount ? totalSuperTiles : std::max<std::size_t>(1, superTileRowsPerChunk * superTileColumnCount);
    const auto effectiveChunkTileRows = superTileRowsPerChunk > std::numeric_limits<std::size_t>::max() / superTileRows ? tileRows : std::min<std::size_t>(static_cast<std::size_t>(tileRows), superTileRowsPerChunk * superTileRows);
    std::atomic<std::size_t> activeWorkers{ 0 };
    std::atomic<std::size_t> maximumActiveWorkers{ 0 };
    std::atomic<std::uint64_t> workerNanoseconds{ 0 };
    const auto conversionStart = std::chrono::steady_clock::now();
    for (std::size_t chunkStart = 0; chunkStart < totalSuperTiles; chunkStart += superTileChunkSize)
    {
        const auto chunkEnd = std::min(totalSuperTiles, chunkStart + superTileChunkSize);
        for (std::size_t batchStart = chunkStart; batchStart < chunkEnd; batchStart += maximumWorkers)
        {
            const auto batchEnd = std::min(chunkEnd, batchStart + maximumWorkers);
            std::vector<std::future<std::size_t>> jobs;
            jobs.reserve(batchEnd - batchStart);
            for (std::size_t superTileIndex = batchStart; superTileIndex < batchEnd; ++superTileIndex)
            {
                jobs.emplace_back(std::async(std::launch::async, [&, superTileIndex] {
                    const auto active = activeWorkers.fetch_add(1, std::memory_order_relaxed) + 1;
                    auto maximum = maximumActiveWorkers.load(std::memory_order_relaxed);
                    while (active > maximum && !maximumActiveWorkers.compare_exchange_weak(maximum, active, std::memory_order_relaxed))
                    {
                    }
                    const auto jobStart = std::chrono::steady_clock::now();
                    try
                    {
                        const auto workerIndex = superTileIndex - batchStart;
                        const auto jobTileCount = materializeSuperTile(superTileIndex, workerCodecs[workerIndex]);
                        workerNanoseconds.fetch_add(static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - jobStart).count()), std::memory_order_relaxed);
                        activeWorkers.fetch_sub(1, std::memory_order_relaxed);
                        return jobTileCount;
                    }
                    catch (...)
                    {
                        activeWorkers.fetch_sub(1, std::memory_order_relaxed);
                        throw;
                    }
                }));
            }

            for (auto& job : jobs)
            {
                completedTiles += job.get();
                if (progress)
                    progress(static_cast<float>(completedTiles) / static_cast<float>(tileCount));
            }
        }
    }

    const auto elapsedMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - conversionStart).count();
    qInfo().noquote() << QStringLiteral("[PointData] Packed tiled conversion: %1 tiles across %2 files in row chunks of %3 tile rows (factor %4); requested workers %5; maximum concurrent workers %6; aggregate worker time %7 ms; elapsed %8 ms")
        .arg(tileCount)
        .arg(totalSuperTiles)
        .arg(effectiveChunkTileRows)
        .arg(chunkTileRows)
        .arg(maximumWorkers)
        .arg(maximumActiveWorkers.load(std::memory_order_relaxed))
        .arg(workerNanoseconds.load(std::memory_order_relaxed) / 1000000)
        .arg(elapsedMilliseconds);

    storage->setPackedTileSource(filePaths, codec->getName(), blocks, superTileIndices, temporaryDirectory);
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
    if (destination == nullptr && totalTiles != 0)
        throw std::invalid_argument("Tiled materialization destination is null");
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

void PointDataTiledStorage::setPackedTileSource(const QStringList& filePaths, const QString& codecName, const std::vector<mv::util::PackedBlobBlock>& blocks, const std::vector<std::size_t>& superTileIndices, std::shared_ptr<void> sourceLifetime, TileLoader loader)
{
    if (!packedTiles())
        throw std::logic_error("Packed tile source requires packed tiled storage");

    if (filePaths.isEmpty() || codecName.isEmpty() || blocks.size() != _descriptors.size() || superTileIndices.size() != _descriptors.size())
        throw std::invalid_argument("Invalid packed tile source metadata");

    if (!loader)
    {
        const auto codec = mv::util::codecRegistry().createCodec(nullptr, codecName);
        auto readers = std::make_shared<std::vector<std::shared_ptr<mv::util::PackedBlobFileReader>>>();
        readers->reserve(filePaths.size());
        for (const auto& filePath : filePaths)
            readers->push_back(std::make_shared<mv::util::PackedBlobFileReader>(filePath, codec));

        loader = [readers, sourceLifetime = std::move(sourceLifetime)](const TileDescriptor& descriptor) {
            if (descriptor.superTileIndex >= readers->size())
                throw std::out_of_range("Packed tile source contains an invalid super-tile index");

            return (*readers)[descriptor.superTileIndex]->read(descriptor.packedBlock);
        };
    }
    else if (sourceLifetime)
    {
        loader = [loader = std::move(loader), sourceLifetime = std::move(sourceLifetime)](const TileDescriptor& descriptor) {
            return loader(descriptor);
        };
    }

    for (std::size_t tileIndex = 0; tileIndex < _descriptors.size(); ++tileIndex)
    {
        if (superTileIndices[tileIndex] >= static_cast<std::size_t>(filePaths.size()))
            throw std::out_of_range("Packed tile source contains an invalid super-tile index");

        _descriptors[tileIndex].packedBlock = blocks[tileIndex];
        _descriptors[tileIndex].superTileIndex = superTileIndices[tileIndex];
    }

    _loader = std::move(loader);
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
    estimate.largestTileBytes = std::min<std::uint64_t>(rows, rowTileSize) *
        std::min<std::uint64_t>(columns, columnTileSize) * elementSize;

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
