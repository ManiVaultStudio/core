#include "PointDataTiledSerialization.h"

#include "PointDataTiledStorage.h"

#include <CoreInterface.h>

#include <ProjectCompressionAction.h>

#include <util/Serialization.h>
#include <util/CodecRegistry.h>

#include <workflow/WorkflowExecutionContext.h>

#include <QDir>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QUuid>
#include <QVector>
#include <QString>

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <deque>
#include <exception>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

namespace
{
struct TileEncodingItem
{
    std::size_t tileIndex = 0;
    mv::pointdata::tiled::TileDescriptor descriptor;
    QByteArray bytes;
    bool hasBytes = false;
};

class TileEncodingQueue
{
public:
    explicit TileEncodingQueue(const std::size_t capacity) :
        _capacity(capacity)
    {
    }

    void push(TileEncodingItem item)
    {
        std::unique_lock lock(_mutex);
        _notFull.wait(lock, [this] {
            return _items.size() < _capacity || _closed || _failure;
        });

        rethrowFailure();

        if (_closed)
            throw std::runtime_error("Tile encoding queue is closed");

        _items.push_back(std::move(item));
        _notEmpty.notify_one();
    }

    bool pop(TileEncodingItem& item)
    {
        std::unique_lock lock(_mutex);
        _notEmpty.wait(lock, [this] {
            return !_items.empty() || _closed || _failure;
        });

        rethrowFailure();

        if (_items.empty())
            return false;

        item = std::move(_items.front());
        _items.pop_front();
        _notFull.notify_one();
        return true;
    }

    void close()
    {
        {
            const std::lock_guard lock(_mutex);
            _closed = true;
        }

        _notEmpty.notify_all();
        _notFull.notify_all();
    }

    void fail(const std::exception_ptr& failure)
    {
        {
            const std::lock_guard lock(_mutex);
            if (!_failure)
                _failure = failure;

            _closed = true;
        }

        _notEmpty.notify_all();
        _notFull.notify_all();
    }

private:
    void rethrowFailure() const
    {
        if (_failure)
            std::rethrow_exception(_failure);
    }

    std::mutex                     _mutex;
    std::condition_variable        _notEmpty;
    std::condition_variable        _notFull;
    std::deque<TileEncodingItem>   _items;
    std::size_t                    _capacity = 0;
    bool                           _closed = false;
    std::exception_ptr             _failure;
};

QVariantMap makeTileMetadata(const mv::pointdata::tiled::TileDescriptor& descriptor)
{
    QVariantMap tile;
    tile["RowOffset"]    = QVariant::fromValue(static_cast<qulonglong>(descriptor.rowOffset));
    tile["ColumnOffset"] = QVariant::fromValue(static_cast<qulonglong>(descriptor.columnOffset));
    tile["Rows"]         = QVariant::fromValue(static_cast<qulonglong>(descriptor.rows));
    tile["Columns"]      = QVariant::fromValue(static_cast<qulonglong>(descriptor.columns));
    return tile;
}

QVariantMap makeEncodedTileVariantMap(TileEncodingItem item)
{
    auto tile = makeTileMetadata(item.descriptor);

    if (item.hasBytes)
    {
        tile["Raw"] = mv::util::bytesToBlobVariantMap(item.bytes.constData(), static_cast<std::uint64_t>(item.bytes.size()), mv::util::BlobStorageLocation::FileInProject);
    }
    else
    {
        tile["Raw"] = item.descriptor.blob;
    }

    return tile;
}

QVariantMap makeTileVariantMap(const mv::pointdata::tiled::PointDataTiledStorage& storage, const std::size_t tileIndex)
{
    const auto& descriptor = storage.descriptors()[tileIndex];

    auto tile = makeTileMetadata(descriptor);

    if (!descriptor.blob.isEmpty())
    {
        tile["Raw"] = descriptor.blob;
    }
    else
    {
        const auto tileView = storage.acquire(tileIndex / storage.tileColumnCount(), tileIndex % storage.tileColumnCount());

        tile["Raw"] = mv::util::bytesToBlobVariantMap(static_cast<const char*>(tileView.data()), tileView.sizeInBytes(), mv::util::BlobStorageLocation::FileInProject);
    }

    return tile;
}

/**
 * Encodes independent tiles with one sequential source reader and a bounded
 * set of compression consumers. Keeping source access in the producer avoids
 * concurrent reads from file-backed datasets while allowing compression and
 * file output to overlap with the next source read.
 */
void encodeTiles(const mv::pointdata::tiled::PointDataTiledStorage& storage, const std::shared_ptr<QVector<QVariantMap>>& tiles, const mv::workflow::SharedWorkflowExecutionContext& executionContext)
{
    const auto tileCount = storage.descriptors().size();
    if (tileCount == 0)
        return;

    executionContext->setProgress(0.0);

    const auto& parallelization = executionContext->getState()->getOptions().execution;
    if (!parallelization.parallel)
    {
        for (std::size_t tileIndex = 0; tileIndex < tileCount; ++tileIndex)
        {
            (*tiles)[static_cast<qsizetype>(tileIndex)] = makeTileVariantMap(storage, tileIndex);
            executionContext->setProgress(static_cast<double>(tileIndex + 1) / static_cast<double>(tileCount));
        }

        return;
    }

    const auto workerCount = std::max<std::size_t>(1, parallelization.maxWorkerThreadCount);
    const auto hardwareThreadCount = std::max(1u, std::thread::hardware_concurrency());
    const auto consumerCount = std::min(tileCount, std::max<std::size_t>(static_cast<std::size_t>(1), std::min(workerCount - 1, static_cast<std::size_t>(hardwareThreadCount))));
    const auto queueCapacity = std::max<std::size_t>(2, std::min<std::size_t>(8, consumerCount * 2));
    const auto progressInterval = std::max<std::size_t>(1, tileCount / 100);
    TileEncodingQueue queue(queueCapacity);
    std::atomic<std::size_t> completedTileCount = 0;
    std::vector<std::thread> consumers;
    consumers.reserve(consumerCount);

    for (std::size_t consumerIndex = 0; consumerIndex < consumerCount; ++consumerIndex)
    {
        consumers.emplace_back([&queue, &tiles, &executionContext, &completedTileCount, tileCount, progressInterval] {
            try
            {
                TileEncodingItem item;
                while (queue.pop(item))
                {
                    (*tiles)[static_cast<qsizetype>(item.tileIndex)] = makeEncodedTileVariantMap(std::move(item));

                    const auto completed = completedTileCount.fetch_add(1) + 1;
                    if (completed == 1 || completed == tileCount || completed % progressInterval == 0)
                        executionContext->setProgress(static_cast<double>(completed) / static_cast<double>(tileCount));
                }
            }
            catch (...)
            {
                queue.fail(std::current_exception());
            }
        });
    }

    try
    {
        for (std::size_t tileIndex = 0; tileIndex < tileCount; ++tileIndex)
        {
            const auto& descriptor = storage.descriptors()[tileIndex];
            TileEncodingItem item;
            item.tileIndex  = tileIndex;
            item.descriptor = descriptor;

            if (descriptor.blob.isEmpty())
            {
                const auto tileView = storage.acquire(tileIndex / storage.tileColumnCount(), tileIndex % storage.tileColumnCount());
                item.bytes = QByteArray(static_cast<const char*>(tileView.data()), static_cast<qsizetype>(tileView.sizeInBytes()));
                item.hasBytes = true;
            }

            queue.push(std::move(item));
        }

        queue.close();
    }
    catch (...)
    {
        queue.fail(std::current_exception());
    }

    for (auto& consumer : consumers)
        consumer.join();

    if (completedTileCount != tileCount)
    {
        try
        {
            TileEncodingItem item;
            queue.pop(item);
        }
        catch (...)
        {
            throw;
        }

        throw std::runtime_error("Tile encoding stopped before all tiles were processed");
    }
}

QVariantMap makeTiledVariantMap(const mv::pointdata::tiled::PointDataTiledStorage& storage, const QVector<QVariantMap>& tiles, const QVector<QVariantMap>& superTiles = {})
{
    const auto memoryEstimate = storage.memoryEstimate();

    QVariantMap result;
    result["Version"]         = storage.packedTiles() ? 4 : 2;
    result["Storage"]         = storage.packedTiles() ? "PackedTileFiles" : "IndexedTileBlobs";
    result["RowTileSize"]     = QVariant::fromValue(static_cast<qulonglong>(storage.rowTileSize()));
    result["ColumnTileSize"]  = QVariant::fromValue(static_cast<qulonglong>(storage.columnTileSize()));
    result["CacheCapacity"]   = QVariant::fromValue(static_cast<qulonglong>(storage.cacheCapacity()));
    result["CacheCapacityBytes"] = QVariant::fromValue(static_cast<qulonglong>(storage.cacheCapacityBytes()));
    result["ElementSize"]     = QVariant::fromValue(static_cast<qulonglong>(storage.elementSize()));
    result["RawDataBytes"]    = QVariant::fromValue(static_cast<qulonglong>(memoryEstimate.rawDataBytes));
    result["TileCount"]       = QVariant::fromValue(static_cast<qulonglong>(memoryEstimate.tileCount));
    result["Tiles"]           = QVariantList(tiles.cbegin(), tiles.cend());

    if (storage.packedTiles())
    {
        result["SuperTileRows"]    = QVariant::fromValue(static_cast<qulonglong>(storage.superTileRows()));
        result["SuperTileColumns"] = QVariant::fromValue(static_cast<qulonglong>(storage.superTileColumns()));
        result["SuperTiles"]       = QVariantList(superTiles.cbegin(), superTiles.cend());
    }

    return result;
}

QVariantMap makeStagedPackedTiledVariantMap(const mv::pointdata::tiled::PointDataTiledStorage& storage, const std::function<void(float)>& progress)
{
    const auto tileRows = storage.tileRowCount();
    const auto tileColumns = storage.tileColumnCount();
    const auto superTileColumnCount = (tileColumns + storage.superTileColumns() - 1) / storage.superTileColumns();
    const auto sourcePaths = storage.packedTileSourcePaths();
    const auto saveDirectory = QDir::cleanPath(mv::projects().getTemporaryDirPath(mv::AbstractProjectManager::TemporaryDirType::Save));
    QVector<QVariantMap> tiles;
    QVector<QVariantMap> superTiles;
    tiles.resize(static_cast<qsizetype>(storage.descriptors().size()));
    superTiles.resize(sourcePaths.size());

    if (!QDir().mkpath(saveDirectory))
        throw std::runtime_error(QString("Unable to create packed tiled save directory: %1").arg(saveDirectory).toStdString());

    std::uint64_t totalBytes = 0;
    for (const auto& sourcePath : sourcePaths)
    {
        const QFileInfo sourceInfo(sourcePath);
        if (!sourceInfo.isFile())
            throw std::runtime_error(QString("Unable to access staged packed supertile: %1").arg(sourcePath).toStdString());

        totalBytes += static_cast<std::uint64_t>(sourceInfo.size());
    }

    std::uint64_t copiedBytes = 0;
    if (progress)
        progress(0.0f);

    for (std::size_t superTileIndex = 0; superTileIndex < static_cast<std::size_t>(sourcePaths.size()); ++superTileIndex)
    {
        const auto fileName = QUuid::createUuid().toString(QUuid::WithoutBraces) + QStringLiteral(".ptpack");
        const auto destinationPath = QDir(saveDirectory).filePath(fileName);
        if (!QFile::copy(sourcePaths[static_cast<qsizetype>(superTileIndex)], destinationPath))
            throw std::runtime_error(QString("Unable to copy staged packed supertile: %1").arg(sourcePaths[static_cast<qsizetype>(superTileIndex)]).toStdString());

        const auto superTileRow = static_cast<std::uint64_t>(superTileIndex) / superTileColumnCount;
        const auto superTileColumn = static_cast<std::uint64_t>(superTileIndex) % superTileColumnCount;
        QVariantMap superTileMap;
        superTileMap["TileRow"] = QVariant::fromValue(static_cast<qulonglong>(superTileRow * storage.superTileRows()));
        superTileMap["TileColumn"] = QVariant::fromValue(static_cast<qulonglong>(superTileColumn * storage.superTileColumns()));
        superTileMap["URI"] = fileName;
        superTileMap["Codec"] = storage.packedTileSourceCodec();
        superTiles[static_cast<qsizetype>(superTileIndex)] = superTileMap;

        copiedBytes += static_cast<std::uint64_t>(QFileInfo(destinationPath).size());
        if (progress)
            progress(totalBytes == 0 ? 1.0f : static_cast<float>(copiedBytes) / static_cast<float>(totalBytes));
    }

    for (std::size_t tileIndex = 0; tileIndex < storage.descriptors().size(); ++tileIndex)
    {
        const auto& descriptor = storage.descriptors()[tileIndex];
        auto tileMap = makeTileMetadata(descriptor);
        tileMap["SuperTileIndex"] = QVariant::fromValue(static_cast<qulonglong>(descriptor.superTileIndex));
        tileMap["PackedTileOffset"] = QVariant::fromValue(static_cast<qulonglong>(descriptor.packedBlock.offset));
        tileMap["PackedTileCompressedSize"] = QVariant::fromValue(static_cast<qulonglong>(descriptor.packedBlock.compressedSize));
        tileMap["PackedTileUncompressedSize"] = QVariant::fromValue(static_cast<qulonglong>(descriptor.packedBlock.uncompressedSize));
        tiles[static_cast<qsizetype>(tileIndex)] = tileMap;
    }

    return makeTiledVariantMap(storage, tiles, superTiles);
}

QVariantMap makePackedTiledVariantMap(const mv::pointdata::tiled::PointDataTiledStorage& storage, const std::function<void(float)>& progress)
{
    if (storage.hasPackedTileSource())
        return makeStagedPackedTiledVariantMap(storage, progress);

    QVector<QVariantMap> tiles;
    QVector<QVariantMap> superTiles;
    const auto tileRows = storage.tileRowCount();
    const auto tileColumns = storage.tileColumnCount();
    const auto tileCount = storage.descriptors().size();
    std::size_t completedTileCount = 0;
    const auto codec = mv::projects().getCurrentProject()->getCompressionAction().createCodec(nullptr);
    const auto saveDirectory = QDir::cleanPath(mv::projects().getTemporaryDirPath(mv::AbstractProjectManager::TemporaryDirType::Save));

    tiles.resize(static_cast<qsizetype>(storage.descriptors().size()));

    for (std::uint64_t superTileRow = 0; superTileRow < tileRows; superTileRow += storage.superTileRows())
    {
        for (std::uint64_t superTileColumn = 0; superTileColumn < tileColumns; superTileColumn += storage.superTileColumns())
        {
            const auto superTileIndex = superTiles.size();
            const auto fileName = QUuid::createUuid().toString(QUuid::WithoutBraces) + QStringLiteral(".ptpack");
            const auto filePath = QDir(saveDirectory).filePath(fileName);
            mv::util::PackedBlobFileWriter writer(filePath, codec);
            const auto lastTileRow = std::min(tileRows, superTileRow + storage.superTileRows());
            const auto lastTileColumn = std::min(tileColumns, superTileColumn + storage.superTileColumns());

            for (std::uint64_t tileRow = superTileRow; tileRow < lastTileRow; ++tileRow)
            {
                for (std::uint64_t tileColumn = superTileColumn; tileColumn < lastTileColumn; ++tileColumn)
                {
                    const auto tileIndex = static_cast<std::size_t>(tileRow * tileColumns + tileColumn);
                    const auto tile = storage.acquire(tileRow, tileColumn);
                    const auto packedBlock = writer.append(static_cast<const char*>(tile.data()), tile.sizeInBytes());

                    const auto& descriptor = storage.descriptors()[tileIndex];
                    QVariantMap tileMap;
                    tileMap["RowOffset"]        = QVariant::fromValue(static_cast<qulonglong>(descriptor.rowOffset));
                    tileMap["ColumnOffset"]     = QVariant::fromValue(static_cast<qulonglong>(descriptor.columnOffset));
                    tileMap["Rows"]             = QVariant::fromValue(static_cast<qulonglong>(descriptor.rows));
                    tileMap["Columns"]          = QVariant::fromValue(static_cast<qulonglong>(descriptor.columns));
                    tileMap["SuperTileIndex"]   = QVariant::fromValue(static_cast<qulonglong>(superTileIndex));
                    tileMap["PackedTileOffset"] = QVariant::fromValue(static_cast<qulonglong>(packedBlock.offset));
                    tileMap["PackedTileCompressedSize"] = QVariant::fromValue(static_cast<qulonglong>(packedBlock.compressedSize));
                    tileMap["PackedTileUncompressedSize"] = QVariant::fromValue(static_cast<qulonglong>(packedBlock.uncompressedSize));
                    tiles[static_cast<qsizetype>(tileIndex)] = tileMap;

                    ++completedTileCount;

                    if (progress)
                        progress(static_cast<float>(completedTileCount) / static_cast<float>(tileCount));
                }
            }

            writer.close();

            QVariantMap superTileMap;
            superTileMap["TileRow"]       = QVariant::fromValue(static_cast<qulonglong>(superTileRow));
            superTileMap["TileColumn"]    = QVariant::fromValue(static_cast<qulonglong>(superTileColumn));
            superTileMap["URI"]           = fileName;
            superTileMap["Codec"]         = codec->getName();
            superTiles.push_back(superTileMap);
        }
    }

    return makeTiledVariantMap(storage, tiles, superTiles);
}
}

namespace mv::pointdata::tiled
{
QVariantMap PointDataTiledSerialization::toVariantMap(const PointDataTiledStorage& storage, const std::function<void(float)>& progress)
{
    if (storage.packedTiles())
        return makePackedTiledVariantMap(storage, progress);

    QVector<QVariantMap> tiles;
    tiles.reserve(static_cast<qsizetype>(storage.descriptors().size()));

    for (std::size_t tileIndex = 0; tileIndex < storage.descriptors().size(); ++tileIndex)
    {
        tiles.push_back(makeTileVariantMap(storage, tileIndex));

        if (progress)
            progress(static_cast<float>(tileIndex + 1) / static_cast<float>(storage.descriptors().size()));
    }

    return makeTiledVariantMap(storage, tiles);
}

mv::workflow::UniqueWorkflowPlan PointDataTiledSerialization::toVariantMapWorkflow(const PointDataTiledStorage& storage)
{
    if (storage.hasPackedTileSource())
    {
        auto plan = std::make_unique<mv::workflow::WorkflowPlan>("Serialize staged packed tiled point data");
        plan->addSequentialStage("Copy staged packed tiled point-data files", [&storage](const mv::workflow::WorkflowPlan::Job&, const mv::workflow::SharedWorkflowExecutionContext& executionContext) {
            executionContext->setOutput(makeStagedPackedTiledVariantMap(storage, [executionContext](const float progress) {
                executionContext->setProgress(progress);
            }));
        }, mv::workflow::WorkflowPlan::JobThreadAffinity::CurrentWorkerThread, std::max(1.0, static_cast<double>(storage.memoryEstimate().rawDataBytes)));
        return plan;
    }

    if (storage.packedTiles())
    {
        const auto tileRows       = storage.tileRowCount();
        const auto tileColumns    = storage.tileColumnCount();
        const auto superTileRows = (tileRows + storage.superTileRows() - 1) / storage.superTileRows();
        const auto superTileColumns = (tileColumns + storage.superTileColumns() - 1) / storage.superTileColumns();
        const auto tileCount      = storage.descriptors().size();
        const auto superTileCount = superTileRows * superTileColumns;
        const auto encodingWeight = std::max(1.0, static_cast<double>(storage.memoryEstimate().rawDataBytes));
        const auto tiles           = std::make_shared<QVector<QVariantMap>>(static_cast<qsizetype>(tileCount));
        const auto superTiles      = std::make_shared<QVector<QVariantMap>>(static_cast<qsizetype>(superTileCount));
        const auto saveDirectory   = QDir::cleanPath(mv::projects().getTemporaryDirPath(mv::AbstractProjectManager::TemporaryDirType::Save));

        auto plan = std::make_unique<mv::workflow::WorkflowPlan>("Serialize packed tiled point data");
        mv::workflow::WorkflowPlan::Jobs jobs;
        jobs.reserve(static_cast<qsizetype>(superTileCount));

        std::size_t superTileIndex = 0;

        for (std::uint64_t superTileRow = 0; superTileRow < tileRows; superTileRow += storage.superTileRows())
        {
            for (std::uint64_t superTileColumn = 0; superTileColumn < tileColumns; superTileColumn += storage.superTileColumns())
            {
                const auto currentSuperTileIndex = superTileIndex++;
                const auto codec = mv::projects().getCurrentProject()->getCompressionAction().createCodec(nullptr);
                const auto lastTileRow = std::min(tileRows, superTileRow + storage.superTileRows());
                const auto lastTileColumn = std::min(tileColumns, superTileColumn + storage.superTileColumns());
                std::uint64_t superTileByteCount = 0;

                for (std::uint64_t tileRow = superTileRow; tileRow < lastTileRow; ++tileRow)
                {
                    for (std::uint64_t tileColumn = superTileColumn; tileColumn < lastTileColumn; ++tileColumn)
                    {
                        const auto tileIndex = static_cast<std::size_t>(tileRow * tileColumns + tileColumn);
                        const auto& descriptor = storage.descriptors()[tileIndex];
                        superTileByteCount += descriptor.rows * descriptor.columns * storage.elementSize();
                    }
                }

                const auto progressByteCount = std::max<std::uint64_t>(1, superTileByteCount);

                jobs.emplace_back(mv::workflow::WorkflowPlan::Job(QString("Encode super tile %1").arg(currentSuperTileIndex), [&storage, tiles, superTiles, saveDirectory, tileRows, tileColumns, currentSuperTileIndex, superTileRow, superTileColumn, lastTileRow, lastTileColumn, superTileByteCount, progressByteCount, codec](const mv::workflow::WorkflowPlan::Job&, const mv::workflow::SharedWorkflowExecutionContext& executionContext) {
                    const auto fileName = QUuid::createUuid().toString(QUuid::WithoutBraces) + QStringLiteral(".ptpack");
                    const auto filePath = QDir(saveDirectory).filePath(fileName);
                    mv::util::PackedBlobFileWriter writer(filePath, codec);
                    std::uint64_t completedByteCount = 0;
                    QElapsedTimer progressTimer;
                    progressTimer.start();

                    executionContext->setProgress(0.0);

                    for (std::uint64_t tileRow = superTileRow; tileRow < lastTileRow; ++tileRow)
                    {
                        for (std::uint64_t tileColumn = superTileColumn; tileColumn < lastTileColumn; ++tileColumn)
                        {
                            const auto tileIndex = static_cast<std::size_t>(tileRow * tileColumns + tileColumn);
                            const auto& descriptor = storage.descriptors()[tileIndex];
                            const auto tileByteCount = descriptor.rows * descriptor.columns * storage.elementSize();
                            const auto tile = storage.acquire(tileRow, tileColumn);
                            const auto packedBlock = writer.append(static_cast<const char*>(tile.data()), tile.sizeInBytes());

                            QVariantMap tileMap;
                            tileMap["RowOffset"] = QVariant::fromValue(static_cast<qulonglong>(descriptor.rowOffset));
                            tileMap["ColumnOffset"] = QVariant::fromValue(static_cast<qulonglong>(descriptor.columnOffset));
                            tileMap["Rows"] = QVariant::fromValue(static_cast<qulonglong>(descriptor.rows));
                            tileMap["Columns"] = QVariant::fromValue(static_cast<qulonglong>(descriptor.columns));
                            tileMap["SuperTileIndex"] = QVariant::fromValue(static_cast<qulonglong>(currentSuperTileIndex));
                            tileMap["PackedTileOffset"] = QVariant::fromValue(static_cast<qulonglong>(packedBlock.offset));
                            tileMap["PackedTileCompressedSize"] = QVariant::fromValue(static_cast<qulonglong>(packedBlock.compressedSize));
                            tileMap["PackedTileUncompressedSize"] = QVariant::fromValue(static_cast<qulonglong>(packedBlock.uncompressedSize));
                            (*tiles)[static_cast<qsizetype>(tileIndex)] = tileMap;

                            completedByteCount += tileByteCount;
                            if (completedByteCount == tileByteCount || completedByteCount == superTileByteCount || progressTimer.elapsed() >= 50)
                            {
                                executionContext->setProgress(static_cast<double>(completedByteCount) / static_cast<double>(progressByteCount));
                                progressTimer.restart();
                            }
                        }
                    }

                    writer.close();

                    QVariantMap superTileMap;
                    superTileMap["TileRow"] = QVariant::fromValue(static_cast<qulonglong>(superTileRow));
                    superTileMap["TileColumn"] = QVariant::fromValue(static_cast<qulonglong>(superTileColumn));
                    superTileMap["URI"] = fileName;
                    superTileMap["Codec"] = codec->getName();
                    (*superTiles)[static_cast<qsizetype>(currentSuperTileIndex)] = superTileMap;
                    executionContext->setProgress(1.0);
                }, mv::workflow::WorkflowPlan::JobThreadAffinity::CurrentWorkerThread, mv::workflow::WorkflowPlan::JobProgressMode::Nested).weighted(std::max(1.0, static_cast<double>(superTileByteCount))));
            }
        }

        if (!jobs.empty())
        {
            plan->addBatchedParallelStage("Encode packed tiled point-data super tiles", std::move(jobs), [](const mv::workflow::SharedWorkflowExecutionContext& executionContext) {
                return executionContext->getState()->getOptions().batching.dataBlockEncodingBatchSize;
            }, encodingWeight);
        }

        plan->addSequentialStage("Publish packed tiled point-data map", [&storage, tiles, superTiles](const mv::workflow::WorkflowPlan::Job&, const mv::workflow::SharedWorkflowExecutionContext& executionContext) {
            executionContext->setOutput(makeTiledVariantMap(storage, *tiles, *superTiles));
        });

        return plan;
    }

    const auto tileCount = storage.descriptors().size();
    const auto encodingWeight = std::max(1.0, static_cast<double>(storage.memoryEstimate().rawDataBytes));
    const auto tiles     = std::make_shared<QVector<QVariantMap>>(static_cast<qsizetype>(tileCount));

    auto plan = std::make_unique<mv::workflow::WorkflowPlan>("Serialize tiled point data");
    plan->addSequentialStage("Encode tiled point-data tiles", [&storage, tiles](const mv::workflow::WorkflowPlan::Job&, const mv::workflow::SharedWorkflowExecutionContext& executionContext) {
        encodeTiles(storage, tiles, executionContext);
    }, mv::workflow::WorkflowPlan::JobThreadAffinity::CurrentWorkerThread, encodingWeight);

    plan->addSequentialStage("Publish tiled point-data map", [&storage, tiles](const mv::workflow::WorkflowPlan::Job&, const mv::workflow::SharedWorkflowExecutionContext& executionContext) {
        executionContext->setOutput(makeTiledVariantMap(storage, *tiles));
    });

    return plan;
}

std::shared_ptr<PointDataTiledStorage> PointDataTiledSerialization::fromVariantMap(const QVariantMap& map, const std::uint64_t rows, const std::uint64_t columns, const std::size_t elementSize)
{
    const auto rowTileSize    = map.value("RowTileSize").toULongLong();
    const auto columnTileSize = map.value("ColumnTileSize").toULongLong();
    const auto cacheCapacity  = map.value("CacheCapacity", QVariant::fromValue<qulonglong>(8)).toULongLong();
    const auto cacheCapacityBytes = map.value("CacheCapacityBytes", QVariant::fromValue<qulonglong>(mv::pointdata::storage::defaultTileCacheCapacityBytes)).toULongLong();
    const auto tileMaps       = map.value("Tiles").toList();
    const auto storageType    = map.value("Storage").toString();
    const auto packedTiles    = storageType == QStringLiteral("SuperTileBlobs") || storageType == QStringLiteral("PackedTileFiles");
    const auto packedTileFiles = storageType == QStringLiteral("PackedTileFiles");

    std::shared_ptr<std::vector<QVariantMap>> superTileBlobs;
    std::shared_ptr<std::vector<std::shared_ptr<QByteArray>>> superTilePayloads;
    std::shared_ptr<std::vector<std::shared_ptr<mv::util::PackedBlobFileReader>>> packedTileReaders;
    if (storageType == QStringLiteral("SuperTileBlobs"))
    {
        superTileBlobs = std::make_shared<std::vector<QVariantMap>>();
        superTilePayloads = std::make_shared<std::vector<std::shared_ptr<QByteArray>>>();

        for (const auto& value : map.value("SuperTiles").toList())
        {
            superTileBlobs->push_back(value.toMap().value("Raw").toMap());
            superTilePayloads->push_back(nullptr);
        }
    }
    else if (packedTileFiles)
    {
        packedTileReaders = std::make_shared<std::vector<std::shared_ptr<mv::util::PackedBlobFileReader>>>();
        for (const auto& value : map.value("SuperTiles").toList())
        {
            const auto superTile = value.toMap();
            const auto fileName   = superTile.value("URI").toString();
            const auto codecName  = superTile.value("Codec").toString();

            if (fileName.isEmpty() || codecName.isEmpty())
                throw std::runtime_error("Packed tiled point-data metadata has an incomplete super-tile file entry");

            const auto codec = mv::util::codecRegistry().createCodec(nullptr, codecName);
            packedTileReaders->push_back(std::make_shared<mv::util::PackedBlobFileReader>(fileName, codec));
        }
    }

    auto storage = std::make_shared<PointDataTiledStorage>(rows, columns, elementSize, rowTileSize, columnTileSize, [packedTiles, packedTileFiles, superTileBlobs, superTilePayloads, packedTileReaders](const TileDescriptor& descriptor) {
        if (!packedTiles)
            return mv::util::bytesFromBlobVariantMap(descriptor.blob);

        if (packedTileFiles)
        {
            if (descriptor.superTileIndex >= packedTileReaders->size())
                throw std::runtime_error("Invalid serialized packed tile file index");

            return (*packedTileReaders)[descriptor.superTileIndex]->read(descriptor.packedBlock);
        }

        if (descriptor.superTileIndex >= superTileBlobs->size())
            throw std::runtime_error("Invalid serialized super-tile index");

        auto& payload = (*superTilePayloads)[descriptor.superTileIndex];
        if (!payload)
            payload = std::make_shared<QByteArray>(mv::util::bytesFromBlobVariantMap((*superTileBlobs)[descriptor.superTileIndex]));

        if (descriptor.superTileOffset + descriptor.superTileSize > static_cast<std::uint64_t>(payload->size()))
        {
            throw std::runtime_error(QString("Serialized super-tile range is invalid for tile (%1, %2): offset %3, size %4, payload size %5")
                .arg(descriptor.rowOffset)
                .arg(descriptor.columnOffset)
                .arg(descriptor.superTileOffset)
                .arg(descriptor.superTileSize)
                .arg(payload->size())
                .toStdString());
        }

        return payload->mid(static_cast<qsizetype>(descriptor.superTileOffset), static_cast<qsizetype>(descriptor.superTileSize));
    });
    storage->setCacheCapacity(static_cast<std::size_t>(cacheCapacity));
    storage->setCacheCapacityBytes(static_cast<std::size_t>(cacheCapacityBytes));

    if (packedTiles)
    {
        storage->setPackedTiles(true, map.value("SuperTileRows", QVariant::fromValue<qulonglong>(1)).toULongLong(), map.value("SuperTileColumns", QVariant::fromValue<qulonglong>(1)).toULongLong());
    }

    auto& descriptors = storage->descriptors();

    for (qsizetype i = 0; i < tileMaps.size() && i < static_cast<qsizetype>(descriptors.size()); ++i)
    {
        const auto tile = tileMaps[i].toMap();

        descriptors[i].blob = tile.value("Raw").toMap();

        if (storageType == QStringLiteral("SuperTileBlobs"))
        {
            descriptors[i].superTileIndex  = tile.value("SuperTileIndex").toULongLong();
            descriptors[i].superTileOffset = tile.value("SuperTileOffset").toULongLong();
            descriptors[i].superTileSize   = tile.value("SuperTileSize").toULongLong();
        }
        else if (packedTileFiles)
        {
            descriptors[i].superTileIndex              = tile.value("SuperTileIndex").toULongLong();
            descriptors[i].packedBlock.offset          = tile.value("PackedTileOffset").toULongLong();
            descriptors[i].packedBlock.compressedSize  = tile.value("PackedTileCompressedSize").toULongLong();
            descriptors[i].packedBlock.uncompressedSize = tile.value("PackedTileUncompressedSize").toULongLong();
        }
    }

    return storage;
}
}
