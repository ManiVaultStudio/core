// SPDX-License-Identifier: LGPL-3.0-or-later

#include "TiledStorageBackendConversionAction.h"

#include <algorithm>
#include <limits>

namespace mv::gui
{

namespace
{

}

TiledStorageBackendConversionAction::TiledStorageBackendConversionAction(QObject* parent, const std::uint64_t rows, const std::uint64_t columns, const std::size_t elementSize, const pointdata::storage::StorageBackendConfiguration& configuration, const bool initializeFromConfiguration) :
    StorageBackendConversionAction(parent, "Tiled storage"),
    _rows(rows),
    _columns(columns),
    _elementSize(elementSize),
    _tileAsymmetryAction(this),
    _tileSizeAction(this, "Tile size (MiB)", 1, 1024, 1),
    _tileGeometryAction(this, "Tile geometry"),
    _cacheCapacityAction(this, "Tile cache capacity", 1, std::numeric_limits<std::int32_t>::max(), 1)
{
    setShowLabels(true);

    _tileGeometryAction.setDefaultWidgetFlags(StringAction::TextEdit);
    _tileGeometryAction.setEnabled(false);
    _tileSizeAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _cacheCapacityAction.setDefaultWidgetFlags(IntegralAction::SpinBox);

    addAction(&_tileAsymmetryAction);
    addAction(&_tileSizeAction);
    addAction(&_tileGeometryAction);
    addAction(&_cacheCapacityAction);

    connect(&_tileAsymmetryAction.getAsymmetryAction(), &IntegralAction::valueChanged, this, [this] {
        updateTileGeometry();
        notifyOptionsChanged();
    });

    connect(&_tileSizeAction, &IntegralAction::valueChanged, this, [this] {
        updateTileGeometry();
        notifyOptionsChanged();
    });

    connect(&_cacheCapacityAction, &IntegralAction::valueChanged, this, [this] {
        notifyOptionsChanged();
    });

    const auto geometry = pointdata::storage::calculateTiledStorageGeometry(_rows, _columns, _elementSize, static_cast<std::uint64_t>(_tileSizeAction.getValue()) * 1024 * 1024, _tileAsymmetryAction.getAsymmetryAction().getValue());
    _cacheCapacityAction.setValue(static_cast<std::int32_t>(pointdata::storage::calculateTiledStorageCacheCapacity(_rows, _columns, geometry)));
    updateTileGeometry();

    if (initializeFromConfiguration)
    {
        _tileSizeAction.setValue(static_cast<std::int32_t>(std::max<std::size_t>(1, configuration.tileSizeBytes / (1024 * 1024))));
        _cacheCapacityAction.setValue(static_cast<std::int32_t>(configuration.cacheCapacity));
    }
}

pointdata::storage::StorageBackendType TiledStorageBackendConversionAction::getTargetBackendType() const
{
    return pointdata::storage::StorageBackendType::Tiled;
}

pointdata::storage::StorageBackendConversionOptionsPtr TiledStorageBackendConversionAction::getConversionOptions() const
{
    auto configuration = std::make_shared<pointdata::storage::StorageBackendConfiguration>();
    const auto geometry = pointdata::storage::calculateTiledStorageGeometry(_rows, _columns, _elementSize, static_cast<std::uint64_t>(_tileSizeAction.getValue()) * 1024 * 1024, _tileAsymmetryAction.getAsymmetryAction().getValue());
    configuration->rowTileSize    = geometry.rowTileSize;
    configuration->columnTileSize = geometry.columnTileSize;
    configuration->tileSizeBytes  = static_cast<std::size_t>(_tileSizeAction.getValue()) * 1024 * 1024;
    configuration->cacheCapacity  = static_cast<std::size_t>(_cacheCapacityAction.getValue());
    return configuration;
}

void TiledStorageBackendConversionAction::updateTileGeometry()
{
    const auto geometry = pointdata::storage::calculateTiledStorageGeometry(_rows, _columns, _elementSize, static_cast<std::uint64_t>(_tileSizeAction.getValue()) * 1024 * 1024, _tileAsymmetryAction.getAsymmetryAction().getValue());
    _cacheCapacityAction.setValue(static_cast<std::int32_t>(pointdata::storage::calculateTiledStorageCacheCapacity(_rows, _columns, geometry)));
    _tileGeometryAction.setString(QString("Rows per tile: %1\nColumns per tile: %2")
        .arg(geometry.rowTileSize)
        .arg(geometry.columnTileSize));
}

}
