#include "PointDataTiledSerialization.h"

#include <util/Serialization.h>

namespace mv::pointdata::tiled
{
QVariantMap PointDataTiledSerialization::toVariantMap(const PointDataTiledStorage& storage)
{
    QVariantMap result;
    result["Version"] = 1;
    result["RowTileSize"] = QVariant::fromValue(static_cast<qulonglong>(storage.rowTileSize()));
    result["ColumnTileSize"] = QVariant::fromValue(static_cast<qulonglong>(storage.columnTileSize()));
    QVariantList tiles;
    for (const auto& descriptor : storage.descriptors()) {
        QVariantMap tile;
        tile["RowOffset"] = QVariant::fromValue(static_cast<qulonglong>(descriptor.rowOffset));
        tile["ColumnOffset"] = QVariant::fromValue(static_cast<qulonglong>(descriptor.columnOffset));
        tile["Rows"] = QVariant::fromValue(static_cast<qulonglong>(descriptor.rows));
        tile["Columns"] = QVariant::fromValue(static_cast<qulonglong>(descriptor.columns));
        tile["Raw"] = descriptor.blob.isEmpty()
            ? mv::util::bytesToBlobVariantMap(descriptor.residentBytes.constData(), descriptor.residentBytes.size())
            : descriptor.blob;
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
