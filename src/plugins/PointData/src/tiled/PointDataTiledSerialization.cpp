#include "PointDataTiledSerialization.h"

#include "PointDataTiledStorage.h"

#include <util/Serialization.h>

#include <workflow/WorkflowExecutionContext.h>

#include <QVector>

namespace
{
QVariantMap makeTileVariantMap(const mv::pointdata::tiled::PointDataTiledStorage& storage, const std::size_t tileIndex)
{
    const auto& descriptor = storage.descriptors()[tileIndex];

    QVariantMap tile;
    tile["RowOffset"]     = QVariant::fromValue(static_cast<qulonglong>(descriptor.rowOffset));
    tile["ColumnOffset"]  = QVariant::fromValue(static_cast<qulonglong>(descriptor.columnOffset));
    tile["Rows"]          = QVariant::fromValue(static_cast<qulonglong>(descriptor.rows));
    tile["Columns"]       = QVariant::fromValue(static_cast<qulonglong>(descriptor.columns));

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

QVariantMap makeTiledVariantMap(const mv::pointdata::tiled::PointDataTiledStorage& storage, const QVector<QVariantMap>& tiles)
{
    const auto memoryEstimate = storage.memoryEstimate();

    QVariantMap result;
    result["Version"]         = 2;
    result["Storage"]         = "IndexedTileBlobs";
    result["RowTileSize"]     = QVariant::fromValue(static_cast<qulonglong>(storage.rowTileSize()));
    result["ColumnTileSize"]  = QVariant::fromValue(static_cast<qulonglong>(storage.columnTileSize()));
    result["CacheCapacity"]   = QVariant::fromValue(static_cast<qulonglong>(storage.cacheCapacity()));
    result["ElementSize"]     = QVariant::fromValue(static_cast<qulonglong>(storage.elementSize()));
    result["RawDataBytes"]    = QVariant::fromValue(static_cast<qulonglong>(memoryEstimate.rawDataBytes));
    result["TileCount"]       = QVariant::fromValue(static_cast<qulonglong>(memoryEstimate.tileCount));
    result["Tiles"]           = QVariantList(tiles.cbegin(), tiles.cend());

    return result;
}
}

namespace mv::pointdata::tiled
{
QVariantMap PointDataTiledSerialization::toVariantMap(const PointDataTiledStorage& storage)
{
    QVector<QVariantMap> tiles;
    tiles.reserve(static_cast<qsizetype>(storage.descriptors().size()));

    for (std::size_t tileIndex = 0; tileIndex < storage.descriptors().size(); ++tileIndex)
        tiles.push_back(makeTileVariantMap(storage, tileIndex));

    return makeTiledVariantMap(storage, tiles);
}

mv::workflow::UniqueWorkflowPlan PointDataTiledSerialization::toVariantMapWorkflow(const PointDataTiledStorage& storage)
{
    const auto tileCount = storage.descriptors().size();
    const auto tiles     = std::make_shared<QVector<QVariantMap>>(static_cast<qsizetype>(tileCount));

    auto plan = std::make_unique<mv::workflow::WorkflowPlan>("Serialize tiled point data");
    mv::workflow::WorkflowPlan::Jobs jobs;
    jobs.reserve(static_cast<qsizetype>(tileCount));

    for (std::size_t tileIndex = 0; tileIndex < tileCount; ++tileIndex)
    {
        jobs.emplace_back(QString("Encode tile %1").arg(tileIndex), [&storage, tiles, tileIndex](const mv::workflow::WorkflowPlan::Job&, const mv::workflow::SharedWorkflowExecutionContext& executionContext) {
            (*tiles)[static_cast<qsizetype>(tileIndex)] = makeTileVariantMap(storage, tileIndex);

            executionContext->setProgress(1.0);
        }, mv::workflow::WorkflowPlan::JobThreadAffinity::CurrentWorkerThread, mv::workflow::WorkflowPlan::JobProgressMode::Atomic);
    }

    if (!jobs.empty())
    {
        plan->addBatchedParallelStage("Encode tiled point-data tiles", std::move(jobs), [](const mv::workflow::SharedWorkflowExecutionContext& executionContext) {
            return executionContext->getState()->getOptions().batching.dataBlockEncodingBatchSize;
        });
    }

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
    const auto tileMaps       = map.value("Tiles").toList();

    auto storage = std::make_shared<PointDataTiledStorage>(rows, columns, elementSize, rowTileSize, columnTileSize, [](const TileDescriptor& descriptor) {
        return mv::util::bytesFromBlobVariantMap(descriptor.blob);
    });
    storage->setCacheCapacity(static_cast<std::size_t>(cacheCapacity));

    auto& descriptors = storage->descriptors();

    for (qsizetype i = 0; i < tileMaps.size() && i < static_cast<qsizetype>(descriptors.size()); ++i)
    {
        const auto tile = tileMaps[i].toMap();

        descriptors[i].blob = tile.value("Raw").toMap();
    }

    return storage;
}
}
