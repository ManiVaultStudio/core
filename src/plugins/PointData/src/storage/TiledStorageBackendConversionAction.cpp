// SPDX-License-Identifier: LGPL-3.0-or-later

#include "TiledStorageBackendConversionAction.h"

#include "PointDataStorageRecommendationDialog.h"

#include <QApplication>
#include <limits>

namespace mv::gui
{

TiledStorageBackendConversionAction::TiledStorageBackendConversionAction(QObject* parent, const std::uint64_t rows, const std::uint64_t columns, const std::size_t elementSize, const pointdata::storage::StorageBackendConfiguration& configuration, const bool initializeFromConfiguration) :
    StorageBackendConversionAction(parent, "Tiled storage"),
    _rows(rows),
    _columns(columns),
    _elementSize(elementSize),
    _tileAsymmetryAction(this),
    _tileSizeAction(this, "Tile size (MiB)", 1, 1024, 1),
    _rowTileSizeAction(this, "Tile height", 1, std::numeric_limits<std::int32_t>::max(), 256),
    _columnTileSizeAction(this, "Tile width", 1, std::numeric_limits<std::int32_t>::max(), 8),
    _cacheCapacityAction(this, "Tile cache capacity", 1, std::numeric_limits<std::int32_t>::max(), 1),
    _packedTilesAction(this, "Packed tiles", false),
    _superTileRowsAction(this, "Super-tile rows", 1, std::numeric_limits<std::int32_t>::max(), 1),
    _superTileColumnsAction(this, "Super-tile columns", 1, std::numeric_limits<std::int32_t>::max(), 1),
    _wizardAction(this, "Wizard")
{
    setShowLabels(true);

    _tileSizeAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _rowTileSizeAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _columnTileSizeAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _cacheCapacityAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _packedTilesAction.setDefaultWidgetFlags(ToggleAction::CheckBox);
    _superTileRowsAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _superTileColumnsAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _wizardAction.setIconByName("lightbulb");

    addAction(&_tileAsymmetryAction);
    addAction(&_tileSizeAction);
    addAction(&_rowTileSizeAction);
    addAction(&_columnTileSizeAction);
    addAction(&_cacheCapacityAction);
    addAction(&_packedTilesAction);
    addAction(&_superTileRowsAction);
    addAction(&_superTileColumnsAction);
    addAction(&_wizardAction);

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

    connect(&_wizardAction, &TriggerAction::triggered, this, [this] {
        auto* dialog = new pointdata::storage::PointDataStorageRecommendationDialog(QApplication::activeWindow(), _rows, _columns, _elementSize);

        if (dialog->exec() == QDialog::Accepted)
        {
            const auto& recommendation = dialog->recommendation();
            _tileAsymmetryAction.getAsymmetryAction().setValue(recommendation.asymmetry);
            _tileSizeAction.setValue(static_cast<std::int32_t>(recommendation.targetTileMiB));
            _rowTileSizeAction.setValue(static_cast<std::int32_t>(recommendation.geometry.rowTileSize));
            _columnTileSizeAction.setValue(static_cast<std::int32_t>(recommendation.geometry.columnTileSize));
            _cacheCapacityAction.setValue(static_cast<std::int32_t>(recommendation.cacheCapacity));
            _packedTilesAction.setChecked(true);
            _superTileRowsAction.setValue(static_cast<std::int32_t>(recommendation.superTileRows));
            _superTileColumnsAction.setValue(static_cast<std::int32_t>(recommendation.superTileColumns));
        }

        dialog->deleteLater();
    });

    updateCacheCapacity();

    if (initializeFromConfiguration)
    {
        _rowTileSizeAction.setValue(static_cast<std::int32_t>(configuration.rowTileSize));
        _columnTileSizeAction.setValue(static_cast<std::int32_t>(configuration.columnTileSize));
        _cacheCapacityAction.setValue(static_cast<std::int32_t>(configuration.cacheCapacity));
        _packedTilesAction.setChecked(configuration.packedTiles);
        _superTileRowsAction.setValue(static_cast<std::int32_t>(configuration.superTileRows));
        _superTileColumnsAction.setValue(static_cast<std::int32_t>(configuration.superTileColumns));
    }

    updatePackedTileControls();
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
    configuration->packedTiles      = _packedTilesAction.isChecked();
    configuration->superTileRows    = static_cast<std::size_t>(_superTileRowsAction.getValue());
    configuration->superTileColumns = static_cast<std::size_t>(_superTileColumnsAction.getValue());
    return configuration;
}

void TiledStorageBackendConversionAction::updatePackedTileControls()
{
    _superTileRowsAction.setEnabled(_packedTilesAction.isChecked());
    _superTileColumnsAction.setEnabled(_packedTilesAction.isChecked());
}

void TiledStorageBackendConversionAction::updateCacheCapacity()
{
    const pointdata::storage::TiledStorageGeometry geometry {
        static_cast<std::size_t>(_rowTileSizeAction.getValue()),
        static_cast<std::size_t>(_columnTileSizeAction.getValue())
    };

    _cacheCapacityAction.setValue(static_cast<std::int32_t>(pointdata::storage::calculateTiledStorageCacheCapacity(_rows, _columns, geometry)));
}

void TiledStorageBackendConversionAction::updateTileGeometryFromPolicy()
{
    const auto geometry = pointdata::storage::calculateTiledStorageGeometry(_rows, _columns, _elementSize, static_cast<std::uint64_t>(_tileSizeAction.getValue()) * 1024 * 1024, _tileAsymmetryAction.getAsymmetryAction().getValue());
    _rowTileSizeAction.setValue(static_cast<std::int32_t>(geometry.rowTileSize));
    _columnTileSizeAction.setValue(static_cast<std::int32_t>(geometry.columnTileSize));
    updateCacheCapacity();
}
}
