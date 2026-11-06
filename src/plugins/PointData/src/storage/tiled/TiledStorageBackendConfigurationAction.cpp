// SPDX-License-Identifier: LGPL-3.0-or-later

#include "TiledStorageBackendConfigurationAction.h"

#include "PointDataTiledStorage.h"

#include <util/Miscellaneous.h>

#include <QSignalBlocker>
#include <QDir>
#include <QStorageInfo>

#include <algorithm>
#include <cmath>
#include <limits>

namespace mv::gui
{

TiledStorageBackendConfigurationAction::TiledStorageBackendConfigurationAction(QObject* parent, const std::uint64_t rows, const std::uint64_t columns, const std::size_t elementSize, const pointdata::storage::StorageBackendConfiguration& configuration, const bool initializeFromConfiguration, const std::optional<std::uint32_t> currentElementTypeIndex, const bool showImportPerformanceOptions, const std::size_t bucketingThreadCount, const std::size_t stagingThreadCount, const std::size_t rowChunkTileFactor, const bool showImportStagingDirectory, const QString& stagingDirectory, const QString& defaultStagingDirectory) :
    StorageBackendConversionAction(parent, "Tiled storage", currentElementTypeIndex),
    _rows(rows),
    _columns(columns),
    _elementSize(elementSize),
    _tileAsymmetryAction(this),
    _tileSizeAction(this, "Target tile size (MiB)", 1, 1024, 1),
    _rowTileSizeAction(this, "Tile height", 1, std::numeric_limits<std::int32_t>::max(), 256),
    _columnTileSizeAction(this, "Tile width", 1, std::numeric_limits<std::int32_t>::max(), 8),
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
    _stagingDirectoryAction(this, "Temporary storage", stagingDirectory),
    _defaultStagingDirectory(defaultStagingDirectory),
    _showImportStagingDirectory(showImportStagingDirectory)
{
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

    if (showImportStagingDirectory)
    {
        addAction(&_stagingDirectoryAction);
        connect(&_stagingDirectoryAction, &DirectoryPickerAction::directoryChanged, this, [this] { updateStagingSpaceObservation(); });
    }

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
    if (showImportStagingDirectory)
        addAction(&_stagingSpaceObservationAction);

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

    if (initializeFromConfiguration)
    {
        _initialConfiguration = getConfiguration();
        _hasInitialConfiguration = true;
    }
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

QString TiledStorageBackendConfigurationAction::getStagingDirectory() const
{
    return _stagingDirectoryAction.getDirectory();
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
    if (!_showImportStagingDirectory)
        return;

    MessageCardAction::ButtonActions buttonActions;
    const auto currentDirectory = QDir::cleanPath(_stagingDirectoryAction.getDirectory());
    const auto defaultDirectory = QDir::cleanPath(_defaultStagingDirectory);
    if (!defaultDirectory.isEmpty() && currentDirectory != defaultDirectory)
        buttonActions.append({ tr("Reset to default"), [this] { _stagingDirectoryAction.setDirectory(_defaultStagingDirectory); } });

    if (_rows == 0 || _columns == 0 || _elementSize == 0 ||
        _rows > std::numeric_limits<std::uint64_t>::max() / _columns ||
        _rows * _columns > std::numeric_limits<std::uint64_t>::max() / _elementSize)
    {
        _stagingSpaceObservationAction.setMessage(util::SeverityLevel::Info, tr("Disk space"), tr("The required staging space could not be estimated."), buttonActions);
        return;
    }

    const auto matrixBytes = _rows * _columns * _elementSize;
    const auto matrixSize = util::getNoBytesHumanReadable(matrixBytes);
    const auto unavailableSpaceDescription = tr("Free space could not be checked for the selected staging directory.");
    QStorageInfo stagingStorage(_stagingDirectoryAction.getDirectory());
    stagingStorage.refresh();
    if (!stagingStorage.isValid() || !stagingStorage.isReady() || stagingStorage.bytesAvailable() < 0)
    {
        _stagingSpaceObservationAction.setMessage(util::SeverityLevel::Warning, tr("Disk space"), unavailableSpaceDescription, buttonActions);
        return;
    }

    const auto availableBytes = static_cast<std::uint64_t>(stagingStorage.bytesAvailable());
    const auto availableSpace = util::getNoBytesHumanReadable(availableBytes);
    if (availableBytes < matrixBytes)
    {
        const auto description = tr("Potential disk-space issue: %1 is available, below the %2 dense-equivalent estimate. Sparse data and compression may reduce actual staging use.")
            .arg(availableSpace, matrixSize);
        _stagingSpaceObservationAction.setMessage(util::SeverityLevel::Warning, tr("Disk space"), description, buttonActions);
    }
    else
    {
        const auto description = tr("%1 is available; the dense-equivalent matrix estimate is %2. Sparse data and compression may reduce actual staging use.")
            .arg(availableSpace, matrixSize);
        _stagingSpaceObservationAction.setMessage(util::SeverityLevel::Info, tr("Disk space"), description, buttonActions);
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
    const auto geometry = pointdata::storage::calculateTiledStorageGeometry(_rows, _columns, _elementSize, static_cast<std::uint64_t>(_tileSizeAction.getValue()) * 1024 * 1024, _tileAsymmetryAction.getAsymmetryAction().getValue());
    _rowTileSizeAction.setValue(static_cast<std::int32_t>(geometry.rowTileSize));
    _columnTileSizeAction.setValue(static_cast<std::int32_t>(geometry.columnTileSize));
    updateCacheCapacity();
}
}

