// SPDX-License-Identifier: LGPL-3.0-or-later

#include "TiledStorageBackendConfigurationAction.h"

#include "PointDataTiledStorage.h"

#include <CoreInterface.h>
#include <util/Miscellaneous.h>

#include <QSignalBlocker>
#include <QStorageInfo>

#include <algorithm>
#include <cmath>
#include <limits>

namespace mv::gui
{

TiledStorageBackendConfigurationAction::TiledStorageBackendConfigurationAction(QObject* parent, const std::uint64_t rows, const std::uint64_t columns, const std::size_t elementSize, const pointdata::storage::StorageBackendConfiguration& configuration, const bool initializeFromConfiguration, const std::optional<std::uint32_t> currentElementTypeIndex, const bool showImportPerformanceOptions, const std::size_t bucketingThreadCount, const std::size_t stagingThreadCount, const std::size_t rowChunkTileFactor, const bool showImportStagingSpaceObservation, const QString& stagingDirectory, const mv::util::ProjectResourceStagingPolicy projectOpenStagingPolicy) :
    StorageBackendConversionAction(parent, "Tiled storage", currentElementTypeIndex),
    _rows(rows),
    _columns(columns),
    _elementSize(elementSize),
    _tileAsymmetryAction(this),
    _deriveTileDimensionsAction(this, "Derive dimensions from target tile size", false),
    _tileSizeAction(this, "Target tile size (MiB)", 1, 1024, 1),
    _rowTileSizeGroupAction(this, "Tile height"),
    _rowTileSizeAction(this, "Tile height", 1, std::numeric_limits<std::int32_t>::max(), 256),
    _rowTileSizePercentageAction(this, "Height (% of rows)", 0.1f, 100.0f, 1.0f, 1),
    _columnTileSizeGroupAction(this, "Tile width"),
    _columnTileSizeAction(this, "Tile width", 1, std::numeric_limits<std::int32_t>::max(), 8),
    _columnTileSizePercentageAction(this, "Width (% of columns)", 0.1f, 100.0f, 1.0f, 1),
    _cacheCapacityAction(this, "Tile cache capacity", 1, std::numeric_limits<std::int32_t>::max(), 1),
    _tileCountObservationAction(this),
    _tileCacheObservationAction(this),
    _stagingSpaceObservationAction(this),
    _packedTilesAction(this, "Packed tiles", false),
    _superTileRowsAction(this, "Super-tile rows", 1, std::numeric_limits<std::int32_t>::max(), 1),
    _superTileColumnsAction(this, "Super-tile columns", 1, std::numeric_limits<std::int32_t>::max(), 1),
    _importPerformanceAction(this, "Import performance"),
    _importPerformanceSummaryAction(&_importPerformanceAction, "Summary"),
    _importPerformanceSettingsAction(&_importPerformanceAction, "Performance settings"),
    _bucketingThreadCountAction(&_importPerformanceSettingsAction, "Bucketing thread count", 1, 32, static_cast<std::int32_t>(std::clamp<std::size_t>(bucketingThreadCount, 1, 32))),
    _stagingThreadCountAction(&_importPerformanceSettingsAction, "Staging thread count", 1, 32, static_cast<std::int32_t>(std::clamp<std::size_t>(stagingThreadCount, 1, 32))),
    _rowChunkTileFactorAction(&_importPerformanceSettingsAction, "CSR chunk tile-height factor", 1, 256, static_cast<std::int32_t>(std::clamp<std::size_t>(rowChunkTileFactor, 1, 256))),
    _projectOpenStagingPolicyAction(this, "Project-open tile staging", { "Inherit project policy", "Archive-backed", "Stage on demand", "Stage before load" }, "Inherit project policy"),
    _stagingDirectory(stagingDirectory),
    _showImportPerformanceOptions(showImportPerformanceOptions),
    _showImportStagingSpaceObservation(showImportStagingSpaceObservation)
{
    _projectOpenStagingPolicyAction.setCurrentText(mv::util::projectResourceStagingPolicyName(projectOpenStagingPolicy) == QStringLiteral("Inherit") ? QStringLiteral("Inherit project policy") : mv::util::projectResourceStagingPolicyName(projectOpenStagingPolicy));
    setShowLabels(true);

    connect(this, &StorageBackendConversionAction::optionsChanged, this, [this] {
        const auto targetElementSize = getTargetElementSize();
        if (!targetElementSize || *targetElementSize == _elementSize)
            return;

        _elementSize = *targetElementSize;
        updateTargetTileSizeFromGeometry();
        updateCacheCapacity();
        updateStorageObservations();
    });

    _tileSizeAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _deriveTileDimensionsAction.setDefaultWidgetFlags(ToggleAction::CheckBox);
    _deriveTileDimensionsAction.setToolTip("When enabled, calculate tile height and width from the target tile size and row-to-column asymmetry. You can still edit the calculated dimensions directly.");
    _rowTileSizeAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _columnTileSizeAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _rowTileSizePercentageAction.setDefaultWidgetFlags(DecimalAction::SpinBox | DecimalAction::Slider);
    _columnTileSizePercentageAction.setDefaultWidgetFlags(DecimalAction::SpinBox | DecimalAction::Slider);
    _rowTileSizeGroupAction.setShowLabels(false);
    _columnTileSizeGroupAction.setShowLabels(false);
    _rowTileSizePercentageAction.setSuffix("%");
    _columnTileSizePercentageAction.setSuffix("%");
    _rowTileSizeAction.setStretch(1);
    _columnTileSizeAction.setStretch(1);
    _rowTileSizePercentageAction.setStretch(2);
    _columnTileSizePercentageAction.setStretch(2);
    // Applying every intermediate value recalculates tile geometry and its
    // storage observations on the GUI thread. Commit the selected value when
    // the user releases the slider instead.
    _rowTileSizePercentageAction.setUpdateDuringDrag(false);
    _columnTileSizePercentageAction.setUpdateDuringDrag(false);
    _cacheCapacityAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _tileCountObservationAction.setToolTip("Heuristic based on the number of resident tile descriptors.");
    _tileCacheObservationAction.setToolTip("Checks whether one decoded tile fits within the configured cache byte limit.");
    _packedTilesAction.setDefaultWidgetFlags(ToggleAction::CheckBox);
    _superTileRowsAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _superTileColumnsAction.setDefaultWidgetFlags(IntegralAction::SpinBox);

    addAction(&_deriveTileDimensionsAction);
    addAction(&_tileAsymmetryAction);
    addAction(&_tileSizeAction);
    _rowTileSizeGroupAction.addAction(&_rowTileSizeAction);
    _rowTileSizeGroupAction.addAction(&_rowTileSizePercentageAction);
    _columnTileSizeGroupAction.addAction(&_columnTileSizeAction);
    _columnTileSizeGroupAction.addAction(&_columnTileSizePercentageAction);
    addAction(&_rowTileSizeGroupAction);
    addAction(&_columnTileSizeGroupAction);
    addAction(&_cacheCapacityAction);
    addAction(&_packedTilesAction);
    addAction(&_superTileRowsAction);
    addAction(&_superTileColumnsAction);

    if (showImportPerformanceOptions)
    {
        _importPerformanceAction.setShowLabels(false);
        _importPerformanceSummaryAction.setDefaultWidgetFlags(StringAction::LineEdit);
        _importPerformanceSummaryAction.setEnabled(false);
        _importPerformanceSummaryAction.setStretch(1);
        _importPerformanceSettingsAction.setShowLabels(true);
        _importPerformanceSettingsAction.setLabelSizingType(LabelSizingType::Auto);
        _importPerformanceSettingsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
        _importPerformanceSettingsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ToolButtonAutoRaise);
        _importPerformanceSettingsAction.setIconByName("sliders");
        _importPerformanceSettingsAction.setToolTip("Edit import performance settings");
        _bucketingThreadCountAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
        _stagingThreadCountAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
        _rowChunkTileFactorAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
        _bucketingThreadCountAction.setToolTip("Maximum worker threads used to bucket sparse CSR entries during HDF5 import.");
        _stagingThreadCountAction.setToolTip("Maximum worker threads used to materialize, compress, and write tiled output during HDF5 import.");
        _rowChunkTileFactorAction.setToolTip("Number of tile heights grouped into each CSR staging chunk during HDF5 import.");

        _importPerformanceSettingsAction.addAction(&_bucketingThreadCountAction);
        _importPerformanceSettingsAction.addAction(&_stagingThreadCountAction);
        _importPerformanceSettingsAction.addAction(&_rowChunkTileFactorAction);
        _importPerformanceAction.addAction(&_importPerformanceSummaryAction);
        _importPerformanceAction.addAction(&_importPerformanceSettingsAction);
        updateImportPerformanceSummary();
        addAction(&_importPerformanceAction);
        _projectOpenStagingPolicyAction.setToolTip("Controls how this dataset's packed tiles are sourced when opening a saved project. This does not change staging during HDF5 import.");
        _projectOpenStagingPolicyAction.setDefaultWidgetFlags(OptionAction::ComboBox);
        addAction(&_projectOpenStagingPolicyAction);

        const auto notifyTuningOptionsChanged = [this] { this->notifyOptionsChanged(); };
        const auto updateTuningOptions = [this, notifyTuningOptionsChanged] {
            updateImportPerformanceSummary();
            notifyTuningOptionsChanged();
        };
        connect(&_bucketingThreadCountAction, &IntegralAction::valueChanged, this, updateTuningOptions);
        connect(&_stagingThreadCountAction, &IntegralAction::valueChanged, this, updateTuningOptions);
        connect(&_rowChunkTileFactorAction, &IntegralAction::valueChanged, this, updateTuningOptions);
    }

    addAction(&_tileCountObservationAction);
    addAction(&_tileCacheObservationAction);
    if (showImportStagingSpaceObservation)
        addAction(&_stagingSpaceObservationAction);

    connect(&_tileAsymmetryAction.getAsymmetryAction(), &IntegralAction::valueChanged, this, [this] {
        if (_deriveTileDimensionsAction.isChecked())
        {
            updateTileGeometryFromPolicy();
            notifyOptionsChanged();
        }
    });

    connect(&_tileSizeAction, &IntegralAction::valueChanged, this, [this] {
        if (_deriveTileDimensionsAction.isChecked())
        {
            updateTileGeometryFromPolicy();
            notifyOptionsChanged();
        }
    });

    connect(&_deriveTileDimensionsAction, &ToggleAction::toggled, this, [this] { updateTileSizingMode(); });

    connect(&_rowTileSizeAction, &IntegralAction::valueChanged, this, [this] {
        if (!_updatingGeometryFromPolicy && _deriveTileDimensionsAction.isChecked())
            _deriveTileDimensionsAction.setChecked(false);
        updateTileSizePercentages();
        updateCacheCapacity();
        notifyOptionsChanged();
    });

    connect(&_columnTileSizeAction, &IntegralAction::valueChanged, this, [this] {
        if (!_updatingGeometryFromPolicy && _deriveTileDimensionsAction.isChecked())
            _deriveTileDimensionsAction.setChecked(false);
        updateTileSizePercentages();
        updateCacheCapacity();
        notifyOptionsChanged();
    });

    connect(&_rowTileSizePercentageAction, &DecimalAction::valueChanged, this, [this] {
        if (!_updatingTileSizePercentage)
            setRowTileSizeFromPercentage();
    });
    connect(&_columnTileSizePercentageAction, &DecimalAction::valueChanged, this, [this] {
        if (!_updatingTileSizePercentage)
            setColumnTileSizeFromPercentage();
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
    updateTileSizePercentages();
    updateTileSizingMode();
    updatePackedTileControls();

    if (initializeFromConfiguration)
    {
        _initialConfiguration = getConfiguration();
        _hasInitialConfiguration = true;
    }
}

void TiledStorageBackendConfigurationAction::fromVariantMap(const QVariantMap& variantMap)
{
    StorageBackendConversionAction::fromVariantMap(variantMap);

    const auto configuration = variantMap.value("TiledConfiguration").toMap();
    const auto restoreAction = [](auto& action, const QVariantMap& parentMap) {
        const auto serializationName = action.getSerializationName();
        if (!parentMap.contains(serializationName))
            return false;

        action.fromVariantMap(parentMap.value(serializationName).toMap());
        return true;
    };
    const auto settingValue = [&variantMap, &configuration](const char* key, const char* legacyKey) {
        const auto currentValue = configuration.value(QLatin1String(key));
        if (currentValue.isValid())
            return currentValue;

        const auto legacyAction = variantMap.value(QLatin1String(legacyKey)).toMap();
        return legacyAction.value("Value");
    };

    // Disable policy-derived geometry while restoring individual action state,
    // then re-enable it at the end if that was the serialized mode.
    const auto deriveDimensionsAction = variantMap.value(_deriveTileDimensionsAction.getSerializationName()).toMap();
    const auto deriveDimensions = deriveDimensionsAction.contains("Value") ?
        deriveDimensionsAction.value("Value") : settingValue("DeriveTileDimensions", "DeriveTileDimensionsAction");
    if (deriveDimensions.isValid())
        _deriveTileDimensionsAction.setChecked(false);

    restoreAction(_tileAsymmetryAction, variantMap);
    restoreAction(_tileSizeAction, variantMap);
    restoreAction(_packedTilesAction, variantMap);
    restoreAction(_superTileRowsAction, variantMap);
    restoreAction(_superTileColumnsAction, variantMap);

    const auto rowTileSizeGroup = variantMap.value(_rowTileSizeGroupAction.getSerializationName()).toMap();
    if (!rowTileSizeGroup.isEmpty())
    {
        _rowTileSizeGroupAction.fromVariantMap(rowTileSizeGroup);
        restoreAction(_rowTileSizePercentageAction, rowTileSizeGroup);
        restoreAction(_rowTileSizeAction, rowTileSizeGroup);
    }

    const auto columnTileSizeGroup = variantMap.value(_columnTileSizeGroupAction.getSerializationName()).toMap();
    if (!columnTileSizeGroup.isEmpty())
    {
        _columnTileSizeGroupAction.fromVariantMap(columnTileSizeGroup);
        restoreAction(_columnTileSizePercentageAction, columnTileSizeGroup);
        restoreAction(_columnTileSizeAction, columnTileSizeGroup);
    }

    const auto importPerformance = variantMap.value(_importPerformanceAction.getSerializationName()).toMap();
    if (!importPerformance.isEmpty())
    {
        _importPerformanceAction.fromVariantMap(importPerformance);
        restoreAction(_importPerformanceSummaryAction, importPerformance);

        const auto performanceSettings = importPerformance.value(_importPerformanceSettingsAction.getSerializationName()).toMap();
        if (!performanceSettings.isEmpty())
        {
            _importPerformanceSettingsAction.fromVariantMap(performanceSettings);
            restoreAction(_bucketingThreadCountAction, performanceSettings);
            restoreAction(_stagingThreadCountAction, performanceSettings);
            restoreAction(_rowChunkTileFactorAction, performanceSettings);
        }
    }
    restoreAction(_projectOpenStagingPolicyAction, variantMap);

    // Accept value-only presets written by the short-lived experimental format.
    const auto restoreLegacyIntegralValue = [&](IntegralAction& action, const char* key, const char* legacyKey) {
        if (variantMap.contains(action.getSerializationName()))
            return;

        const auto value = settingValue(key, legacyKey);
        if (value.isValid())
            action.setValue(value.toInt());
    };

    const auto restoreLegacyToggleValue = [&](ToggleAction& action, const char* key, const char* legacyKey) {
        if (variantMap.contains(action.getSerializationName()))
            return;

        const auto value = settingValue(key, legacyKey);
        if (value.isValid())
            action.setChecked(value.toBool());
    };

    restoreLegacyIntegralValue(_tileAsymmetryAction.getAsymmetryAction(), "TileAsymmetry", "TileAsymmetryAction");
    restoreLegacyIntegralValue(_tileSizeAction, "TileSizeMiB", "TileSizeAction");
    restoreLegacyIntegralValue(_rowTileSizeAction, "TileHeight", "RowTileSizeAction");
    restoreLegacyIntegralValue(_columnTileSizeAction, "TileWidth", "ColumnTileSizeAction");
    restoreLegacyToggleValue(_packedTilesAction, "PackedTiles", "PackedTilesAction");
    restoreLegacyIntegralValue(_superTileRowsAction, "SuperTileRows", "SuperTileRowsAction");
    restoreLegacyIntegralValue(_superTileColumnsAction, "SuperTileColumns", "SuperTileColumnsAction");
    restoreLegacyIntegralValue(_bucketingThreadCountAction, "BucketingThreadCount", "BucketingThreadCountAction");
    restoreLegacyIntegralValue(_stagingThreadCountAction, "StagingThreadCount", "StagingThreadCountAction");
    restoreLegacyIntegralValue(_rowChunkTileFactorAction, "RowChunkTileFactor", "RowChunkTileFactorAction");

    if (deriveDimensionsAction.contains("Value"))
        restoreAction(_deriveTileDimensionsAction, variantMap);
    else if (deriveDimensions.isValid())
        _deriveTileDimensionsAction.setChecked(deriveDimensions.toBool());

    updateTileSizePercentages();
    updateTileSizingMode();

    // Tile-cache capacity can be overridden by the user, although geometry
    // changes normally reset it to a suggested value. Restore the preset value
    // after geometry policy updates so the explicit override is preserved.
    restoreAction(_cacheCapacityAction, variantMap);
    restoreLegacyIntegralValue(_cacheCapacityAction, "CacheCapacity", "CacheCapacityAction");

    updatePackedTileControls();
    updateImportPerformanceSummary();
    updateStorageObservations();
    notifyOptionsChanged();
}

QVariantMap TiledStorageBackendConfigurationAction::toVariantMap() const
{
    auto variantMap = StorageBackendConversionAction::toVariantMap();

    _tileAsymmetryAction.insertIntoVariantMap(variantMap);
    _deriveTileDimensionsAction.insertIntoVariantMap(variantMap);
    _tileSizeAction.insertIntoVariantMap(variantMap);
    _cacheCapacityAction.insertIntoVariantMap(variantMap);
    _packedTilesAction.insertIntoVariantMap(variantMap);
    _superTileRowsAction.insertIntoVariantMap(variantMap);
    _superTileColumnsAction.insertIntoVariantMap(variantMap);

    auto rowTileSizeGroup = _rowTileSizeGroupAction.toVariantMap();
    _rowTileSizeAction.insertIntoVariantMap(rowTileSizeGroup);
    _rowTileSizePercentageAction.insertIntoVariantMap(rowTileSizeGroup);
    variantMap.insert(_rowTileSizeGroupAction.getSerializationName(), rowTileSizeGroup);

    auto columnTileSizeGroup = _columnTileSizeGroupAction.toVariantMap();
    _columnTileSizeAction.insertIntoVariantMap(columnTileSizeGroup);
    _columnTileSizePercentageAction.insertIntoVariantMap(columnTileSizeGroup);
    variantMap.insert(_columnTileSizeGroupAction.getSerializationName(), columnTileSizeGroup);

    if (_showImportPerformanceOptions)
    {
        auto performanceSettings = _importPerformanceSettingsAction.toVariantMap();
        _bucketingThreadCountAction.insertIntoVariantMap(performanceSettings);
        _stagingThreadCountAction.insertIntoVariantMap(performanceSettings);
        _rowChunkTileFactorAction.insertIntoVariantMap(performanceSettings);

        auto importPerformance = _importPerformanceAction.toVariantMap();
        _importPerformanceSummaryAction.insertIntoVariantMap(importPerformance);
        importPerformance.insert(_importPerformanceSettingsAction.getSerializationName(), performanceSettings);
        variantMap.insert(_importPerformanceAction.getSerializationName(), importPerformance);
        _projectOpenStagingPolicyAction.insertIntoVariantMap(variantMap);
    }

    return variantMap;
}

mv::util::ProjectResourceStagingPolicy TiledStorageBackendConfigurationAction::getProjectOpenStagingPolicy() const
{
    if (_projectOpenStagingPolicyAction.getCurrentText() == QStringLiteral("Archive-backed"))
        return mv::util::ProjectResourceStagingPolicy::ArchiveBacked;
    if (_projectOpenStagingPolicyAction.getCurrentText() == QStringLiteral("Stage on demand"))
        return mv::util::ProjectResourceStagingPolicy::OnDemand;
    if (_projectOpenStagingPolicyAction.getCurrentText() == QStringLiteral("Stage before load"))
        return mv::util::ProjectResourceStagingPolicy::BeforeLoad;
    return mv::util::ProjectResourceStagingPolicy::Inherit;
}

void TiledStorageBackendConfigurationAction::updateTileSizePercentages()
{
    const auto updatePercentage = [this](DecimalAction& percentageAction, const std::int32_t tileSize, const std::uint64_t dimension) {
        if (dimension == 0)
        {
            percentageAction.setEnabled(false);
            return;
        }

        percentageAction.setEnabled(true);
        const auto percentage = static_cast<float>(std::clamp(static_cast<long double>(tileSize) * 100.0L / static_cast<long double>(dimension), 0.1L, 100.0L));
        _updatingTileSizePercentage = true;
        percentageAction.setValue(percentage);
        _updatingTileSizePercentage = false;
    };

    updatePercentage(_rowTileSizePercentageAction, _rowTileSizeAction.getValue(), _rows);
    updatePercentage(_columnTileSizePercentageAction, _columnTileSizeAction.getValue(), _columns);
}

void TiledStorageBackendConfigurationAction::setRowTileSizeFromPercentage()
{
    if (_rows == 0)
        return;

    const auto tileSize = std::clamp(std::llround(static_cast<long double>(_rows) * _rowTileSizePercentageAction.getValue() / 100.0L), 1LL, static_cast<long long>(std::numeric_limits<std::int32_t>::max()));
    _rowTileSizeAction.setValue(static_cast<std::int32_t>(tileSize));
}

void TiledStorageBackendConfigurationAction::setColumnTileSizeFromPercentage()
{
    if (_columns == 0)
        return;

    const auto tileSize = std::clamp(std::llround(static_cast<long double>(_columns) * _columnTileSizePercentageAction.getValue() / 100.0L), 1LL, static_cast<long long>(std::numeric_limits<std::int32_t>::max()));
    _columnTileSizeAction.setValue(static_cast<std::int32_t>(tileSize));
}

void TiledStorageBackendConfigurationAction::updateTileSizingMode()
{
    const auto deriveGeometry = _deriveTileDimensionsAction.isChecked();
    _tileAsymmetryAction.setEnabled(deriveGeometry);
    _tileSizeAction.setEnabled(deriveGeometry);

    if (deriveGeometry)
        updateTileGeometryFromPolicy();
}

pointdata::storage::StorageBackendType TiledStorageBackendConfigurationAction::getTargetBackendType() const
{
    return pointdata::storage::StorageBackendType::Tiled;
}

pointdata::storage::StorageBackendConfigurationOptionsPtr TiledStorageBackendConfigurationAction::getConversionOptions() const
{
    auto configuration = std::make_shared<pointdata::storage::StorageBackendConfiguration>(getConfiguration());
    configuration->targetElementTypeIndex = getTargetElementTypeIndex();
    return configuration;
}

bool TiledStorageBackendConfigurationAction::hasChanges() const
{
    if (StorageBackendConversionAction::hasChanges())
        return true;

    if (!_hasInitialConfiguration)
        return false;

    const auto configuration = getConfiguration();
    return configuration.rowTileSize != _initialConfiguration.rowTileSize ||
        configuration.columnTileSize != _initialConfiguration.columnTileSize ||
        configuration.cacheCapacity != _initialConfiguration.cacheCapacity ||
        configuration.cacheCapacityBytes != _initialConfiguration.cacheCapacityBytes ||
        configuration.packedTiles != _initialConfiguration.packedTiles ||
        configuration.superTileRows != _initialConfiguration.superTileRows ||
        configuration.superTileColumns != _initialConfiguration.superTileColumns;
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

std::size_t TiledStorageBackendConfigurationAction::getBucketingThreadCount() const
{
    return static_cast<std::size_t>(_bucketingThreadCountAction.getValue());
}

std::size_t TiledStorageBackendConfigurationAction::getStagingThreadCount() const
{
    return static_cast<std::size_t>(_stagingThreadCountAction.getValue());
}

std::size_t TiledStorageBackendConfigurationAction::getRowChunkTileFactor() const
{
    return static_cast<std::size_t>(_rowChunkTileFactorAction.getValue());
}

void TiledStorageBackendConfigurationAction::updateImportPerformanceSummary()
{
    _importPerformanceSummaryAction.setString(QStringLiteral("%1 bucket threads, %2 staging threads, CSR chunk tile-height factor %3")
        .arg(_bucketingThreadCountAction.getValue())
        .arg(_stagingThreadCountAction.getValue())
        .arg(_rowChunkTileFactorAction.getValue()));
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
        _tileCountObservationAction.setMessage(util::SeverityLevel::Info, "Tile count", "Tile count and memory estimates are unavailable until dataset dimensions and element size are known.");
        _tileCacheObservationAction.setMessage(util::SeverityLevel::Info, "Tile cache", "Tile size and cache fit cannot be estimated until dataset dimensions and element size are known.");
        updateStagingSpaceObservation();
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

    _tileCountObservationAction.setMessage(tileSeverity, "Tile count", tileDescription);

    const auto cacheSeverity = memoryEstimate.largestTileBytes > _cacheCapacityBytes ? util::SeverityLevel::Warning : util::SeverityLevel::Info;
    auto cacheDescription = QString("A tile is estimated at up to %1; the cache byte limit is %2.")
        .arg(util::getNoBytesHumanReadable(memoryEstimate.largestTileBytes))
        .arg(util::getNoBytesHumanReadable(_cacheCapacityBytes));

    if (cacheSeverity == util::SeverityLevel::Warning)
        cacheDescription += " A tile larger than this limit is immediately evicted, so repeated access may reload it.";

    _tileCacheObservationAction.setMessage(cacheSeverity, "Tile cache", cacheDescription);
    updateStagingSpaceObservation();
}

void TiledStorageBackendConfigurationAction::updateStagingSpaceObservation()
{
    if (!_showImportStagingSpaceObservation)
        return;

    if (_rows == 0 || _columns == 0 || _elementSize == 0 ||
        _rows > std::numeric_limits<std::uint64_t>::max() / _columns ||
        _rows * _columns > std::numeric_limits<std::uint64_t>::max() / _elementSize)
    {
        _stagingSpaceObservationAction.setMessage(util::SeverityLevel::Info, tr("Disk space"), tr("The required staging space could not be estimated."));
        return;
    }

    const auto matrixBytes = _rows * _columns * _elementSize;
    const auto matrixSize = util::getNoBytesHumanReadable(matrixBytes);
    const auto settingsButtonAction = MessageCardAction::ButtonAction { tr("Temporary settings"), [] { mv::settings().getEditSettingsAction().trigger(); } };
    const auto unavailableSpaceDescription = tr("Free space could not be checked for the app-managed staging directory: %1.").arg(_stagingDirectory.toHtmlEscaped());
    QStorageInfo stagingStorage(_stagingDirectory);
    stagingStorage.refresh();
    if (!stagingStorage.isValid() || !stagingStorage.isReady() || stagingStorage.bytesAvailable() < 0)
    {
        _stagingSpaceObservationAction.setMessage(util::SeverityLevel::Warning, tr("Disk space"), unavailableSpaceDescription, { settingsButtonAction });
        return;
    }

    const auto availableBytes = static_cast<std::uint64_t>(stagingStorage.bytesAvailable());
    const auto availableSpace = util::getNoBytesHumanReadable(availableBytes);
    if (availableBytes < matrixBytes)
    {
        const auto description = tr("Potentially insufficient space in the app-managed staging directory (%1): %2 is available, below the %3 dense-equivalent estimate. Sparse data and compression may reduce actual staging use. Changing the application temporary directory takes effect after restart.")
            .arg(_stagingDirectory.toHtmlEscaped(), availableSpace, matrixSize);
        _stagingSpaceObservationAction.setMessage(util::SeverityLevel::Warning, tr("Disk space"), description, { settingsButtonAction });
    }
    else
    {
        const auto description = tr("The app-managed staging directory is %1. %2 is available; the dense-equivalent matrix estimate is %3. Sparse data and compression may reduce actual staging use.")
            .arg(_stagingDirectory.toHtmlEscaped(), availableSpace, matrixSize);
        _stagingSpaceObservationAction.setMessage(util::SeverityLevel::Info, tr("Disk space"), description);
    }
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
    _updatingGeometryFromPolicy = true;
    const auto geometry = pointdata::storage::calculateTiledStorageGeometry(_rows, _columns, _elementSize, static_cast<std::uint64_t>(_tileSizeAction.getValue()) * 1024 * 1024, _tileAsymmetryAction.getAsymmetryAction().getValue());
    _rowTileSizeAction.setValue(static_cast<std::int32_t>(geometry.rowTileSize));
    _columnTileSizeAction.setValue(static_cast<std::int32_t>(geometry.columnTileSize));
    _updatingGeometryFromPolicy = false;
    updateTileSizePercentages();
    updateCacheCapacity();
}
}

