// SPDX-License-Identifier: LGPL-3.0-or-later

#include "TiledStorageBackendConfigurationAction.h"

#include "../tiled/PointDataTiledStorage.h"

#include <util/Miscellaneous.h>

#include <QSignalBlocker>

#include <algorithm>
#include <cmath>
#include <limits>

namespace mv::gui
{

TiledStorageBackendConfigurationAction::TiledStorageBackendConfigurationAction(QObject* parent, const std::uint64_t rows, const std::uint64_t columns, const std::size_t elementSize, const pointdata::storage::StorageBackendConfiguration& configuration, const bool initializeFromConfiguration) :
    StorageBackendConversionAction(parent, "Tiled storage"),
    _rows(rows),
    _columns(columns),
    _elementSize(elementSize),
    _tileAsymmetryAction(this),
    _tileSizeAction(this, "Target tile size (MiB)", 1, 1024, 1),
    _rowTileSizeAction(this, "Tile height", 1, std::numeric_limits<std::int32_t>::max(), 256),
    _columnTileSizeAction(this, "Tile width", 1, std::numeric_limits<std::int32_t>::max(), 8),
    _cacheCapacityAction(this, "Tile cache capacity", 1, std::numeric_limits<std::int32_t>::max(), 1),
    _tileCountObservationAction(this, "Tile count"),
    _tileCacheObservationAction(this, "Tile cache"),
    _packedTilesAction(this, "Packed tiles", false),
    _superTileRowsAction(this, "Super-tile rows", 1, std::numeric_limits<std::int32_t>::max(), 1),
    _superTileColumnsAction(this, "Super-tile columns", 1, std::numeric_limits<std::int32_t>::max(), 1)
{
    setShowLabels(true);

    _tileSizeAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _rowTileSizeAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _columnTileSizeAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _cacheCapacityAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _tileCountObservationAction.setToolTip("Heuristic based on the number of resident tile descriptors.");
    _tileCacheObservationAction.setToolTip("Checks whether one decoded tile fits within the configured cache byte limit.");
    _packedTilesAction.setDefaultWidgetFlags(ToggleAction::CheckBox);
    _superTileRowsAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _superTileColumnsAction.setDefaultWidgetFlags(IntegralAction::SpinBox);

    addAction(&_tileAsymmetryAction);
    addAction(&_tileSizeAction);
    addAction(&_rowTileSizeAction);
    addAction(&_columnTileSizeAction);
    addAction(&_cacheCapacityAction);
    addAction(&_packedTilesAction);
    addAction(&_superTileRowsAction);
    addAction(&_superTileColumnsAction);
    addAction(&_tileCountObservationAction);
    addAction(&_tileCacheObservationAction);

    connect(&_tileAsymmetryAction.getAsymmetryAction(), &IntegralAction::valueChanged, this, [this] {
        updateTileGeometryFromPolicy();
        notifyOptionsChanged();
    });

    connect(&_tileSizeAction, &IntegralAction::valueChanged, this, [this] {
        updateTileGeometryFromPolicy();
        notifyOptionsChanged();
    });

    connect(&_rowTileSizeAction, &IntegralAction::valueChanged, this, [this] {
        updateCacheCapacity();
        notifyOptionsChanged();
    });

    connect(&_columnTileSizeAction, &IntegralAction::valueChanged, this, [this] {
        updateCacheCapacity();
        notifyOptionsChanged();
    });

    connect(&_cacheCapacityAction, &IntegralAction::valueChanged, this, [this] {
        updateStorageObservations();
        notifyOptionsChanged();
    });

    connect(&_packedTilesAction, &ToggleAction::toggled, this, [this] {
        updatePackedTileControls();
        notifyOptionsChanged();
    });

    connect(&_superTileRowsAction, &IntegralAction::valueChanged, this, [this] {
        notifyOptionsChanged();
    });

    connect(&_superTileColumnsAction, &IntegralAction::valueChanged, this, [this] {
        notifyOptionsChanged();
    });

    if (initializeFromConfiguration)
    {
        _cacheCapacityBytes = configuration.cacheCapacityBytes;
        _rowTileSizeAction.setValue(static_cast<std::int32_t>(configuration.rowTileSize));
        _columnTileSizeAction.setValue(static_cast<std::int32_t>(configuration.columnTileSize));
        updateTargetTileSizeFromGeometry();
        _cacheCapacityAction.setValue(static_cast<std::int32_t>(configuration.cacheCapacity));
        _packedTilesAction.setChecked(configuration.packedTiles);
        _superTileRowsAction.setValue(static_cast<std::int32_t>(configuration.superTileRows));
        _superTileColumnsAction.setValue(static_cast<std::int32_t>(configuration.superTileColumns));
    }
    else
    {
        updateTileGeometryFromPolicy();
    }

    updateStorageObservations();
    updatePackedTileControls();
}

pointdata::storage::StorageBackendType TiledStorageBackendConfigurationAction::getTargetBackendType() const
{
    return pointdata::storage::StorageBackendType::Tiled;
}

pointdata::storage::StorageBackendConversionOptionsPtr TiledStorageBackendConfigurationAction::getConversionOptions() const
{
    return std::make_shared<pointdata::storage::StorageBackendConfiguration>(getConfiguration());
}

pointdata::storage::StorageBackendConfiguration TiledStorageBackendConfigurationAction::getConfiguration() const
{
    pointdata::storage::StorageBackendConfiguration configuration;
    configuration.rowTileSize        = static_cast<std::size_t>(_rowTileSizeAction.getValue());
    configuration.columnTileSize     = static_cast<std::size_t>(_columnTileSizeAction.getValue());
    configuration.cacheCapacity      = static_cast<std::size_t>(_cacheCapacityAction.getValue());
    configuration.cacheCapacityBytes = _cacheCapacityBytes;
    configuration.packedTiles        = _packedTilesAction.isChecked();
    configuration.superTileRows      = static_cast<std::size_t>(_superTileRowsAction.getValue());
    configuration.superTileColumns   = static_cast<std::size_t>(_superTileColumnsAction.getValue());
    return configuration;
}

void TiledStorageBackendConfigurationAction::updatePackedTileControls()
{
    _superTileRowsAction.setEnabled(_packedTilesAction.isChecked());
    _superTileColumnsAction.setEnabled(_packedTilesAction.isChecked());
}

void TiledStorageBackendConfigurationAction::updateCacheCapacity()
{
    const pointdata::storage::TiledStorageGeometry geometry {
        static_cast<std::size_t>(_rowTileSizeAction.getValue()),
        static_cast<std::size_t>(_columnTileSizeAction.getValue())
    };

    _cacheCapacityAction.setValue(static_cast<std::int32_t>(pointdata::storage::calculateTiledStorageCacheCapacity(_rows, _columns, geometry)));
    updateStorageObservations();
}

void TiledStorageBackendConfigurationAction::updateStorageObservations()
{
    if (_rows == 0 || _columns == 0 || _elementSize == 0)
    {
        _tileCountObservationAction.setObservation(util::SeverityLevel::Info, "Tile count and memory estimates are unavailable until dataset dimensions and element size are known.");
        _tileCacheObservationAction.setObservation(util::SeverityLevel::Info, "Tile size and cache fit cannot be estimated until dataset dimensions and element size are known.");
        return;
    }

    const auto memoryEstimate = pointdata::tiled::PointDataTiledStorage::estimateMemory(
        _rows,
        _columns,
        _elementSize,
        static_cast<std::size_t>(_rowTileSizeAction.getValue()),
        static_cast<std::size_t>(_columnTileSizeAction.getValue()),
        static_cast<std::size_t>(_cacheCapacityAction.getValue()),
        _cacheCapacityBytes);
    const auto tileSeverity = memoryEstimate.tileCount >= 100000 ? util::SeverityLevel::Warning : util::SeverityLevel::Info;
    auto tileDescription = QString("The layout uses %1 tiles; the resident tile index is estimated at %2.")
        .arg(memoryEstimate.tileCount)
        .arg(util::getNoBytesHumanReadable(memoryEstimate.indexBytes));

    if (tileSeverity == util::SeverityLevel::Warning)
        tileDescription += " Many small tiles can increase index memory and setup time. Consider increasing tile height or width.";

    _tileCountObservationAction.setObservation(tileSeverity, tileDescription);

    const auto cacheSeverity = memoryEstimate.largestTileBytes > _cacheCapacityBytes ? util::SeverityLevel::Warning : util::SeverityLevel::Info;
    auto cacheDescription = QString("A tile is estimated at up to %1; the cache byte limit is %2.")
        .arg(util::getNoBytesHumanReadable(memoryEstimate.largestTileBytes))
        .arg(util::getNoBytesHumanReadable(_cacheCapacityBytes));

    if (cacheSeverity == util::SeverityLevel::Warning)
        cacheDescription += " A tile larger than this limit is immediately evicted, so repeated access may reload it.";

    _tileCacheObservationAction.setObservation(cacheSeverity, cacheDescription);
}

void TiledStorageBackendConfigurationAction::updateTargetTileSizeFromGeometry()
{
    const auto tileBytes = static_cast<long double>(_rowTileSizeAction.getValue()) * _columnTileSizeAction.getValue() * _elementSize;
    const auto targetSizeMiB = std::clamp(std::llround(std::min(tileBytes / (1024.0L * 1024.0L), 1024.0L)), 1LL, 1024LL);
    const QSignalBlocker signalBlocker(&_tileSizeAction);

    _tileSizeAction.setValue(static_cast<std::int32_t>(targetSizeMiB));
}

void TiledStorageBackendConfigurationAction::updateTileGeometryFromPolicy()
{
    const auto geometry = pointdata::storage::calculateTiledStorageGeometry(_rows, _columns, _elementSize, static_cast<std::uint64_t>(_tileSizeAction.getValue()) * 1024 * 1024, _tileAsymmetryAction.getAsymmetryAction().getValue());
    _rowTileSizeAction.setValue(static_cast<std::int32_t>(geometry.rowTileSize));
    _columnTileSizeAction.setValue(static_cast<std::int32_t>(geometry.columnTileSize));
    updateCacheCapacity();
}
}

