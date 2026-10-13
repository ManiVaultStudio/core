#include "PointDataTiledSerialization.h"

#include <util/Serialization.h>

namespace mv::pointdata::tiled
{
QVariantMap PointDataTiledSerialization::toVariantMap(const PointDataTiledStorage& storage)
{
    QVariantMap result;
    // Version 2 is an indexed tile representation. Tile payloads are explicitly
    // stored as external project blobs; the project JSON contains only metadata
    // and the per-tile blob index. Version 1 remains readable below.
    result["Version"] = 2;
    result["Storage"] = "IndexedTileBlobs";
    result["RowTileSize"] = QVariant::fromValue(static_cast<qulonglong>(storage.rowTileSize()));
    result["ColumnTileSize"] = QVariant::fromValue(static_cast<qulonglong>(storage.columnTileSize()));
    result["ElementSize"] = QVariant::fromValue(static_cast<qulonglong>(storage.elementSize()));
    result["RawDataBytes"] = QVariant::fromValue(static_cast<qulonglong>(storage.memoryEstimate().rawDataBytes));
    result["TileCount"] = QVariant::fromValue(static_cast<qulonglong>(storage.memoryEstimate().tileCount));
    QVariantList tiles;
    tiles.reserve(static_cast<qsizetype>(storage.descriptors().size()));
    for (std::size_t tileIndex = 0; tileIndex < storage.descriptors().size(); ++tileIndex) {
        const auto& descriptor = storage.descriptors()[tileIndex];
        QVariantMap tile;
        tile["RowOffset"] = QVariant::fromValue(static_cast<qulonglong>(descriptor.rowOffset));
        tile["ColumnOffset"] = QVariant::fromValue(static_cast<qulonglong>(descriptor.columnOffset));
        tile["Rows"] = QVariant::fromValue(static_cast<qulonglong>(descriptor.rows));
        tile["Columns"] = QVariant::fromValue(static_cast<qulonglong>(descriptor.columns));
        if (!descriptor.blob.isEmpty())
            tile["Raw"] = descriptor.blob;
        else {
            const auto tileView = storage.acquire(tileIndex / storage.tileColumnCount(), tileIndex % storage.tileColumnCount());
            tile["Raw"] = mv::util::bytesToBlobVariantMap(static_cast<const char*>(tileView.data()), tileView.sizeInBytes(),
                mv::util::BlobStorageLocation::FileInProject);
        }
        tiles.push_back(tile);
    }
    result["Tiles"] = tiles;
    return result;
}

std::shared_ptr<PointDataTiledStorage> PointDataTiledSerialization::fromVariantMap(const QVariantMap& map,
    const std::uint64_t rows, const std::uint64_t columns, const std::size_t elementSize)
{
    const auto rowTileSize = map.value("RowTileSize").toULongLong();
    const auto columnTileSize = map.value("ColumnTileSize").toULongLong();
    const auto tileMaps = map.value("Tiles").toList();
    auto storage = std::make_shared<PointDataTiledStorage>(rows, columns, elementSize, rowTileSize, columnTileSize,
        [](const TileDescriptor& descriptor) { return mv::util::bytesFromBlobVariantMap(descriptor.blob); });
    auto& descriptors = storage->descriptors();
    for (qsizetype i = 0; i < tileMaps.size() && i < static_cast<qsizetype>(descriptors.size()); ++i) {
        const auto tile = tileMaps[i].toMap();
        descriptors[i].blob = tile.value("Raw").toMap();
    }
    return storage;
}
}
