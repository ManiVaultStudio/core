// SPDX-License-Identifier: LGPL-3.0-or-later

#include "TiledStorageBackendConversionAction.h"

#include <limits>

namespace mv::gui
{

TiledStorageBackendConversionAction::TiledStorageBackendConversionAction(QObject* parent) :
    StorageBackendConversionAction(parent, "Tiled storage"),
    _rowTileSizeAction(this, "Rows per tile", 1, std::numeric_limits<std::int32_t>::max(), 256),
    _columnTileSizeAction(this, "Columns per tile", 1, std::numeric_limits<std::int32_t>::max(), 8),
    _cacheCapacityAction(this, "Tile cache capacity", 1, std::numeric_limits<std::int32_t>::max(), 8)
{
    setShowLabels(true);

    _rowTileSizeAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _columnTileSizeAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _cacheCapacityAction.setDefaultWidgetFlags(IntegralAction::SpinBox);

    addAction(&_rowTileSizeAction);
    addAction(&_columnTileSizeAction);
    addAction(&_cacheCapacityAction);
}

pointdata::storage::StorageBackendType TiledStorageBackendConversionAction::getTargetBackendType() const
{
    return pointdata::storage::StorageBackendType::Tiled;
}

pointdata::storage::StorageBackendConversionOptionsPtr TiledStorageBackendConversionAction::getConversionOptions() const
{
    auto configuration = std::make_shared<pointdata::storage::StorageBackendConfiguration>();
    configuration->rowTileSize    = static_cast<std::size_t>(_rowTileSizeAction.getValue());
    configuration->columnTileSize = static_cast<std::size_t>(_columnTileSizeAction.getValue());
    configuration->cacheCapacity  = static_cast<std::size_t>(_cacheCapacityAction.getValue());
    return configuration;
}

}
