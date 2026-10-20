#include "PointDataTiledSerialization.h"

#include "PointDataTiledStorage.h"

#include <CoreInterface.h>

#include <ProjectCompressionAction.h>

#include <util/Serialization.h>
#include <util/CodecRegistry.h>

#include <workflow/WorkflowExecutionContext.h>

#include <QDir>
#include <QUuid>
#include <QVector>
#include <QString>

#include <memory>
#include <stdexcept>
#include <vector>

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

QVariantMap makeTiledVariantMap(const mv::pointdata::tiled::PointDataTiledStorage& storage, const QVector<QVariantMap>& tiles, const QVector<QVariantMap>& superTiles = {})
{
    const auto memoryEstimate = storage.memoryEstimate();

    QVariantMap result;
    result["Version"]         = storage.packedTiles() ? 4 : 2;
    result["Storage"]         = storage.packedTiles() ? "PackedTileFiles" : "IndexedTileBlobs";
    result["RowTileSize"]     = QVariant::fromValue(static_cast<qulonglong>(storage.rowTileSize()));
    result["ColumnTileSize"]  = QVariant::fromValue(static_cast<qulonglong>(storage.columnTileSize()));
    result["CacheCapacity"]   = QVariant::fromValue(static_cast<qulonglong>(storage.cacheCapacity()));
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

QVariantMap makePackedTiledVariantMap(const mv::pointdata::tiled::PointDataTiledStorage& storage)
{
    QVector<QVariantMap> tiles;
    QVector<QVariantMap> superTiles;
    const auto tileRows = storage.tileRowCount();
    const auto tileColumns = storage.tileColumnCount();
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
QVariantMap PointDataTiledSerialization::toVariantMap(const PointDataTiledStorage& storage)
{
    if (storage.packedTiles())
        return makePackedTiledVariantMap(storage);

    QVector<QVariantMap> tiles;
    tiles.reserve(static_cast<qsizetype>(storage.descriptors().size()));

    for (std::size_t tileIndex = 0; tileIndex < storage.descriptors().size(); ++tileIndex)
        tiles.push_back(makeTileVariantMap(storage, tileIndex));

    return makeTiledVariantMap(storage, tiles);
}

mv::workflow::UniqueWorkflowPlan PointDataTiledSerialization::toVariantMapWorkflow(const PointDataTiledStorage& storage)
{
    if (storage.packedTiles())
    {
        auto plan = std::make_unique<mv::workflow::WorkflowPlan>("Serialize packed tiled point data");
        plan->addSequentialStage("Encode tiled point-data super tiles", [&storage](const mv::workflow::WorkflowPlan::Job&, const mv::workflow::SharedWorkflowExecutionContext& executionContext) {
            executionContext->setOutput(PointDataTiledSerialization::toVariantMap(storage));
        });
        return plan;
    }

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
