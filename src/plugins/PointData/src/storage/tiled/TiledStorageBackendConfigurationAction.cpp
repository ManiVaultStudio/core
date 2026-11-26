// SPDX-License-Identifier: LGPL-3.0-or-later

#include "TiledStorageBackendConfigurationAction.h"

#include "PointDataTiledStorage.h"

#include <Application.h>
#include <CoreInterface.h>
#include <Project.h>
#include <util/BlobCodec.h>
#include <util/CodecRegistry.h>
#include <util/Miscellaneous.h>
#include <util/StyledIcon.h>

#include <QSignalBlocker>
#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QPalette>
#include <QStorageInfo>

#include <algorithm>
#include <cmath>
#include <limits>
#include <thread>

namespace mv::gui
{

namespace
{
QString projectTileCodecName()
{
    auto* project = mv::projects().getCurrentProject();
    if (!project)
        return mv::util::BlobCodec::typeToString(mv::util::BlobCodec::Type::Zstd);

    return mv::util::BlobCodec::typeToString(mv::util::codecRegistry().typeFromDisplayName(project->getCompressionAction().getCodecTypeAction().getCurrentText()));
}

QStringList tileCodecOptions()
{
    return {
        mv::util::codecRegistry().factory(mv::util::BlobCodec::Type::None).displayName(),
        mv::util::codecRegistry().factory(mv::util::BlobCodec::Type::Zstd).displayName()
    };
}
}

TiledStorageBackendConfigurationAction::TiledStorageBackendConfigurationAction(QObject* parent, const std::uint64_t rows, const std::uint64_t columns, const std::size_t elementSize, const pointdata::storage::StorageBackendConfiguration& configuration, const bool initializeFromConfiguration, const std::optional<std::uint32_t> currentElementTypeIndex, const bool showImportStagingSpaceObservation, const QString& stagingDirectory) :
    StorageBackendConversionAction(parent, "Tiled storage", currentElementTypeIndex),
    _rows(rows),
    _columns(columns),
    _elementSize(elementSize),
    _developerOptionsGroupAction(this, "General options"),
    _performanceOptionsGroupAction(this, "Performance options"),
    _compressionSettingsAction(this, "Compression settings"),
    _superTileOptionsAction(this, "Supertile options"),
    _stripLayoutOptionsAction(this, "Strip layout options"),
    _estimateBalancedLayoutAction(this, "Estimate balanced layout"),
    _rowTileSizeGroupAction(this, "Tile height"),
    _squareTilesAction(this, "Square tiles", false),
    _rowTileSizeAction(this, "Tile height", 1, std::numeric_limits<std::int32_t>::max(), 256),
    _rowTileSizePercentageAction(this, "Height (% of rows)", 0.1f, 100.0f, 1.0f, 2),
    _columnTileSizeGroupAction(this, "Tile width"),
    _columnTileSizeAction(this, "Tile width", 1, std::numeric_limits<std::int32_t>::max(), 8),
    _columnTileSizePercentageAction(this, "Width (% of columns)", 0.1f, 100.0f, 1.0f, 2),
    _tileCountAction(this, "Tile count"),
    _superTileCountAction(this, "Supertile count"),
    _tileByteSizeAction(this, "Tile byte size estimate"),
    _superTileByteSizeAction(this, "Supertile byte size estimate"),
    _cacheByteSizeAction(this, "Cache byte size estimate"),
    _diskSpaceAction(this, "Disk space estimate"),
    _cacheCapacityAction(this, "Tile cache capacity", 1, std::numeric_limits<std::int32_t>::max(), 1),
    _cacheCapacityBytesAction(this, "Tile cache byte limit", 1, 65536, 512),
    _tileCodecAction(this, "Packed tile codec"),
    _packedTilesAction(this, "Packed tiles", false),
    _rowStripHeightAction(this, "Rows per row strip", 1, std::numeric_limits<std::int32_t>::max(), 32),
    _columnStripWidthAction(this, "Columns per column strip", 1, std::numeric_limits<std::int32_t>::max(), 32),
    _superTileRowsAction(this, "Super-tile rows", 1, std::numeric_limits<std::int32_t>::max(), 1),
    _superTileColumnsAction(this, "Super-tile columns", 1, std::numeric_limits<std::int32_t>::max(), 1),
    _importPerformanceAction(this, "Import performance"),
    _importPerformanceSummaryAction(&_importPerformanceAction, "Summary"),
    _importPerformanceSettingsAction(&_importPerformanceAction, "Performance settings"),
    _bucketingThreadCountAction(&_importPerformanceSettingsAction, "Bucketing thread count", 1, 32, 4),
    _stagingThreadCountAction(&_importPerformanceSettingsAction, "Staging thread count", 1, 32, 4),
    _rowChunkTileFactorAction(&_importPerformanceSettingsAction, "Chunk tile-height factor", 1, 256, 3),
    _projectResourceStagingPolicyAction(this, "Resource staging policy", { "Inherit", "Read ranges on demand", "Stage on first read", "Stage immediately" }, "Inherit"),
    _zstdCompressionLevelAction(this, "Zstandard compression level", 1, 22, std::clamp(configuration.tileCompressionLevel, 1, 22)),
    _importBenchmarkEnabledAction(this, "Enable import benchmarking", false),
    _importBenchmarkDirectoryAction(this, "Import benchmark location"),
    _initialProjectResourceStagingPolicy(mv::util::ProjectResourceStagingPolicy::Inherit),
    _stagingDirectory(stagingDirectory),
    _showImportPerformanceOptions(false),
    _showImportStagingSpaceObservation(showImportStagingSpaceObservation)
{
    const auto projectResourceStagingPolicy = mv::util::ProjectResourceStagingPolicy::Inherit;
    _projectResourceStagingPolicyAction.setSerializationName("Project-open tile staging");
    _projectResourceStagingPolicyAction.setCurrentText(mv::util::projectResourceStagingPolicyName(projectResourceStagingPolicy));
    _projectResourceStagingPolicyAction.setDefaultWidgetFlags(OptionAction::ComboBox);
    _projectResourceStagingPolicyAction.setToolTip("Select the policy stored on the imported dataset for its archive-backed resources. It takes effect when a saved project is opened; it does not change staging during HDF5 import.");
    _projectResourceStagingPolicyAction.setDescription(
        "<p>Stores the imported dataset's policy for archive-backed resources in the project. It applies when the saved project is opened, not while HDF5 data is being imported. Inherit uses the project's resource staging policy.</p>");
    updateProjectResourceStagingPolicyOption();
    const auto initialTileCodecName = configuration.tileCodecName.isEmpty() ? projectTileCodecName() : configuration.tileCodecName;
    _tileCodecAction.initialize(tileCodecOptions(), mv::util::codecRegistry().factory(initialTileCodecName).displayName());
    _tileCodecAction.setSerializationName("Packed tile codec");
    _tileCodecAction.setDefaultWidgetFlags(OptionAction::ComboBox);
    _tileCodecAction.setToolTip("Choose how each packed tile payload is encoded. This choice is stored with the dataset and used when tiles are read later.");
    _tileCodecAction.setDescription(
        "<p>Select no compression or Zstandard for packed tile payloads. The selected codec is stored with this dataset and used to decode its tiles when the project is reopened. Changing the codec for an existing packed dataset causes its tiles to be re-encoded the next time the project is saved.</p>");
    _tileCodecAction.setEnabled(false);
    _zstdCompressionLevelAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _zstdCompressionLevelAction.setToolTip("Controls the tradeoff between packed-tile encoding speed and file size. Level 3 is a balanced default.");
    _zstdCompressionLevelAction.setDescription(
        "<p>Controls how much CPU time Zstandard spends compressing packed tiles during import or conversion. Lower levels (1-3) are faster and usually produce larger files; higher levels (up to 22) take longer and may produce smaller files. Level 3 is a good general-purpose setting. This affects encoding time and disk usage, not the stored values.</p>");

    connect(this, &StorageBackendConversionAction::optionsChanged, this, [this] {
        const auto targetElementSize = getTargetElementSize();
        if (!targetElementSize || *targetElementSize == _elementSize)
            return;

        _elementSize = *targetElementSize;
        updateCacheCapacity();
    });

    getSettingsGroupAction().setText("Layout settings");
    getSettingsGroupAction().setIconByName("table");
    getSettingsGroupAction().setLabelSizingType(GroupAction::LabelSizingType::Fixed);
    getChecksGroupAction().setIconByName("list-check");
    _estimateBalancedLayoutAction.setDefaultWidgetFlags(TriggerAction::Text);
    _estimateBalancedLayoutAction.setToolTip("Choose a balanced starting layout with square tiles. This heuristic aims for moderate tile sizes; benchmark with your workload before settling on the layout.");
    _estimateBalancedLayoutAction.setDescription("<p>Estimates square tile dimensions from dataset size and element type, then groups up to 16 tiles per supertile. Square estimates are easier to reason about and provide a neutral starting point for row and column access, but are not a performance guarantee.</p>");
    _tileCountAction.setDefaultWidgetFlags(StringAction::LineEdit);
    _tileCountAction.setEnabled(false);
    _tileCountAction.getTrailingAction().setEnabled(false);
    _tileCountAction.getTrailingAction().setVisible(false);
    _rowTileSizeAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _columnTileSizeAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _squareTilesAction.setDefaultWidgetFlags(ToggleAction::CheckBox);
    _squareTilesAction.setToolTip("Keep tile height and width equal. Changing either dimension updates the other.");
    _squareTilesAction.setDescription("<p>Keeps the number of rows and columns per tile equal. Changing either tile dimension updates the other.</p>");
    _rowTileSizePercentageAction.setDefaultWidgetFlags(DecimalAction::SpinBox | DecimalAction::Slider);
    _columnTileSizePercentageAction.setDefaultWidgetFlags(DecimalAction::SpinBox | DecimalAction::Slider);
    _rowTileSizeGroupAction.setShowLabels(false);
    _columnTileSizeGroupAction.setShowLabels(false);
    _rowTileSizeGroupAction.setDescription(
        "<p>Set the number of dataset rows in each tile, directly or as a percentage of the dataset's row count. Larger tiles can help contiguous row access but may fetch more unrelated data for random access.</p>");
    _columnTileSizeGroupAction.setDescription(
        "<p>Set the number of dimensions in each tile, directly or as a percentage of the dataset's column count. Wider tiles can help column-range access but may fetch more unrelated data for narrow requests.</p>");
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
    _cacheCapacityBytesAction.setSuffix(" MiB");
    _cacheCapacityBytesAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _cacheCapacityBytesAction.setDescription("<p>Maximum decoded tile-cache memory in addition to the tile-count capacity. The effective cache is limited by whichever bound is reached first. A tile larger than this limit cannot remain cached.</p>");
    _cacheCapacityAction.setDescription(
        "<p>Tile-count ceiling for decoded data kept in memory. This suggested value is recalculated when the tile layout changes. The byte limit below is an additional ceiling; whichever limit is reached first controls cache use.</p>");
    _tileCountAction.setDescription("<p>Estimated number of tiles. Very high counts increase index memory and can increase import and conversion time.</p>");
    _superTileCountAction.setDescription("<p>Estimated number of packed containers for the selected tile and supertile dimensions. Fewer supertiles reduce file overhead.</p>");
    _tileByteSizeAction.setDescription("<p>Estimated uncompressed size of one tile. Large tiles need more memory per read and may exceed the cache byte limit.</p>");
    _superTileByteSizeAction.setDescription("<p>Estimated uncompressed size of one packed supertile. Large supertiles can increase read amplification and temporary memory use.</p>");
    _cacheByteSizeAction.setDescription("<p>Estimated decoded bytes that may be resident, bounded by both tile cache capacity and the byte limit.</p>");
    _diskSpaceAction.setDescription("<p>Available space in the staging directory compared with the matrix's dense-equivalent size. Sparse data and compression can reduce actual usage.</p>");
    for (auto* estimateAction : { &_tileCountAction, &_superTileCountAction, &_tileByteSizeAction, &_superTileByteSizeAction, &_cacheByteSizeAction, &_diskSpaceAction })
    {
        estimateAction->setDefaultWidgetFlags(StringAction::LineEdit);
        estimateAction->setEnabled(false);
        estimateAction->getTrailingAction().setEnabled(false);
        estimateAction->getTrailingAction().setVisible(false);
    }
    _packedTilesAction.setDefaultWidgetFlags(ToggleAction::CheckBox);
    _packedTilesAction.setToolTip("Store groups of tiles together in packed supertile files.");
    _packedTilesAction.setDescription(
        "<p>Groups tiles into supertile files to reduce per-file and metadata overhead. The supertile row and column settings below define each group. When disabled, tiles are stored individually.</p>");
    _rowStripHeightAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _columnStripWidthAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _rowStripHeightAction.setToolTip("Number of matrix rows stored in each independently compressed row strip.");
    _columnStripWidthAction.setToolTip("Number of matrix columns stored in each independently compressed column strip.");
    _rowStripHeightAction.setDescription("<p>Rows per strip controls how many adjacent matrix rows are stored together. Larger strips reduce container overhead, but may read extra rows for a small request.</p>");
    _columnStripWidthAction.setDescription("<p>Columns per strip controls how many adjacent matrix columns are stored together. Larger strips reduce container overhead, but may read extra columns for a narrow request.</p>");
    _superTileRowsAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _superTileColumnsAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _superTileRowsAction.setDescription(
        "<p>Number of tile rows grouped into each packed container file. Larger values can reduce file and metadata overhead, but may increase the amount of data read for an individual tile request.</p>");
    _superTileColumnsAction.setDescription(
        "<p>Number of tile columns grouped into each packed container file. Larger values can reduce file and metadata overhead, but may increase the amount of data read for an individual tile request.</p>");

    const auto configureOptionsGroup = [](VerticalGroupAction& groupAction, const QString& toolTip, const QString& description) {
        groupAction.setShowLabels(true);
        groupAction.setLabelSizingType(GroupAction::LabelSizingType::Fixed);
        groupAction.setIconByName("code");
        groupAction.setToolTip(toolTip);
        groupAction.setDescription(description);
    };
    configureOptionsGroup(
        _compressionSettingsAction,
        "Configure packed-tile encoding and compression effort.",
        "<p>Choose the codec for packed tile payloads and, during H5AD import, the Zstandard compression level. Higher compression levels can reduce file size at the cost of import time.</p>");
    configureOptionsGroup(
        _superTileOptionsAction,
        "Configure whether tiles are packed together and the supertile dimensions.",
        "<p>Supertile settings control how neighboring tiles are grouped into packed files. Grouping reduces file overhead, while larger groups may increase read amplification.</p>");
    configureOptionsGroup(
        _stripLayoutOptionsAction,
        "Configure row and column strip dimensions.",
        "<p>Set the number of rows or columns stored in each independently compressed strip. These dimensions affect range access and the amount of unrelated data read for a request.</p>");

    addAction(&_squareTilesAction);
    addAction(&_rowTileSizeGroupAction);
    addAction(&_columnTileSizeGroupAction);
    addAction(&_estimateBalancedLayoutAction);
    _rowTileSizeGroupAction.addAction(&_rowTileSizeAction);
    _rowTileSizeGroupAction.addAction(&_rowTileSizePercentageAction);
    _columnTileSizeGroupAction.addAction(&_columnTileSizeAction);
    _columnTileSizeGroupAction.addAction(&_columnTileSizePercentageAction);
    _developerOptionsGroupAction.setText("General options");
    _developerOptionsGroupAction.setIconByName("code");
    _developerOptionsGroupAction.setLabelSizingType(GroupAction::LabelSizingType::Fixed);
    addGroupAction(&_developerOptionsGroupAction, false);
    _performanceOptionsGroupAction.setShowLabels(true);
    _performanceOptionsGroupAction.setLabelSizingType(GroupAction::LabelSizingType::Fixed);
    _performanceOptionsGroupAction.setIconByName("code");
    _performanceOptionsGroupAction.setDescription(
        "<p>Controls worker count and tile-row chunk size during tiled storage conversion. More workers may improve throughput but use more CPU and memory.</p>");
    addGroupAction(&_performanceOptionsGroupAction, false);
    addGroupAction(&_compressionSettingsAction, false);
    addGroupAction(&_superTileOptionsAction, false);
    addGroupAction(&_stripLayoutOptionsAction, false);
    _developerOptionsGroupAction.addAction(&_cacheCapacityAction);
    _developerOptionsGroupAction.addAction(&_cacheCapacityBytesAction);
    _compressionSettingsAction.addAction(&_tileCodecAction);
    _compressionSettingsAction.addAction(&_zstdCompressionLevelAction);
    _superTileOptionsAction.addAction(&_packedTilesAction);
    _superTileOptionsAction.addAction(&_superTileRowsAction);
    _superTileOptionsAction.addAction(&_superTileColumnsAction);
    _stripLayoutOptionsAction.addAction(&_rowStripHeightAction);
    _stripLayoutOptionsAction.addAction(&_columnStripWidthAction);

    auto& developerModeAction = mv::settings().getDeveloperSettingsAction().getEnableDeveloperAddonsAction();
    const auto updateDeveloperOptionsVisibility = [this, &developerModeAction] {
        const auto visible = developerModeAction.isChecked();
        setGroupActionVisibility(&_developerOptionsGroupAction, visible);
        setGroupActionVisibility(&_performanceOptionsGroupAction, visible && (_showImportPerformanceOptions || _showConversionPerformanceOptions));
        setGroupActionVisibility(&_compressionSettingsAction, visible);
        setGroupActionVisibility(&_superTileOptionsAction, visible);
        setGroupActionVisibility(&_stripLayoutOptionsAction, visible);
    };
    connect(&developerModeAction, &ToggleAction::toggled, this, updateDeveloperOptionsVisibility);
    updateDeveloperOptionsVisibility();

    if (_showImportPerformanceOptions)
    {
        _importBenchmarkEnabledAction.setDescription("<p>Write timing and throughput details from this HDF5 import to a JSON report. Benchmarking adds a small amount of measurement overhead.</p>");
        _importBenchmarkDirectoryAction.setDescription("<p>Folder where the benchmark report will be written. A uniquely timestamped JSON file is created there after import.</p>");
        _importBenchmarkDirectoryAction.setEnabled(_importBenchmarkEnabledAction.isChecked());
        _developerOptionsGroupAction.addAction(&_importBenchmarkEnabledAction);
        _developerOptionsGroupAction.addAction(&_importBenchmarkDirectoryAction);
        connect(&_importBenchmarkEnabledAction, &ToggleAction::toggled, this, [this] {
            _importBenchmarkDirectoryAction.setEnabled(_importBenchmarkEnabledAction.isChecked());
            notifyOptionsChanged();
        });
        connect(&_importBenchmarkDirectoryAction, &DirectoryPickerAction::directoryChanged, this, [this](const QString&) {
            notifyOptionsChanged();
        });
    }

    if (_showImportPerformanceOptions)
    {
        _importPerformanceAction.setShowLabels(false);
        _importPerformanceSummaryAction.setDefaultWidgetFlags(StringAction::LineEdit);
        _importPerformanceSummaryAction.setEnabled(false);
        _importPerformanceSummaryAction.setStretch(1);
        _importPerformanceSettingsAction.setShowLabels(true);
        _importPerformanceSettingsAction.setLabelSizingType(GroupAction::LabelSizingType::Fixed);
        _importPerformanceSettingsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
        _importPerformanceSettingsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ToolButtonAutoRaise);
        _importPerformanceSettingsAction.setIconByName("sliders");
        _importPerformanceSettingsAction.setToolTip("Edit import performance settings");
        _importPerformanceSettingsAction.setDescription(
            "<p>Advanced HDF5 import controls for sparse-entry bucketing, tiled output staging, and CSR chunk grouping. Higher worker counts may improve throughput but use more CPU and memory.</p>");
        _bucketingThreadCountAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
        _stagingThreadCountAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
        _rowChunkTileFactorAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
        _bucketingThreadCountAction.setDescription("<p>Maximum worker threads used to bucket sparse CSR entries during HDF5 import. More workers can improve throughput but use more CPU and memory.</p>");
        _stagingThreadCountAction.setDescription("<p>Maximum worker threads used to materialize, compress, and write tiled output during HDF5 import. More workers can improve throughput but use more CPU and memory.</p>");
        _rowChunkTileFactorAction.setDescription("<p>Number of tile heights grouped into each CSR staging chunk during HDF5 import. Larger chunks can reduce scheduling overhead but require more staging memory.</p>");

        _importPerformanceSettingsAction.addAction(&_bucketingThreadCountAction);
        _importPerformanceSettingsAction.addAction(&_stagingThreadCountAction);
        _importPerformanceSettingsAction.addAction(&_rowChunkTileFactorAction);
        _performanceOptionsGroupAction.addAction(&_bucketingThreadCountAction);
        _performanceOptionsGroupAction.addAction(&_stagingThreadCountAction);
        _performanceOptionsGroupAction.addAction(&_rowChunkTileFactorAction);
        _importPerformanceAction.addAction(&_importPerformanceSummaryAction);
        _importPerformanceAction.addAction(&_importPerformanceSettingsAction);
        _developerOptionsGroupAction.addAction(&_projectResourceStagingPolicyAction);
        updateImportPerformanceSummary();
        const auto notifyTuningOptionsChanged = [this] { this->notifyOptionsChanged(); };
        const auto updateTuningOptions = [this, notifyTuningOptionsChanged] {
            updateImportPerformanceSummary();
            notifyTuningOptionsChanged();
        };
        connect(&_bucketingThreadCountAction, &IntegralAction::valueChanged, this, updateTuningOptions);
        connect(&_stagingThreadCountAction, &IntegralAction::valueChanged, this, updateTuningOptions);
        connect(&_rowChunkTileFactorAction, &IntegralAction::valueChanged, this, updateTuningOptions);
        connect(&_projectResourceStagingPolicyAction, &OptionAction::currentIndexChanged, this, notifyTuningOptionsChanged);
    }

    connect(&_rowStripHeightAction, &IntegralAction::valueChanged, this, [this] { notifyOptionsChanged(); });
    connect(&_columnStripWidthAction, &IntegralAction::valueChanged, this, [this] { notifyOptionsChanged(); });

    addCheckAction(&_tileCountAction);
    addCheckAction(&_superTileCountAction);
    addCheckAction(&_tileByteSizeAction);
    addCheckAction(&_superTileByteSizeAction);
    addCheckAction(&_cacheByteSizeAction);
    if (showImportStagingSpaceObservation)
        addCheckAction(&_diskSpaceAction);
    moveElementTypeAnalysisActionToEnd();

    connect(&_estimateBalancedLayoutAction, &TriggerAction::triggered, this, [this] { estimateBalancedLayout(); });

    connect(&_squareTilesAction, &ToggleAction::toggled, this, [this] { synchronizeSquareTileSize(true); });

    connect(&_rowTileSizeAction, &IntegralAction::valueChanged, this, [this] {
        synchronizeSquareTileSize(true);
    });

    connect(&_columnTileSizeAction, &IntegralAction::valueChanged, this, [this] {
        synchronizeSquareTileSize(false);
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

    connect(&_cacheCapacityBytesAction, &IntegralAction::valueChanged, this, [this](const std::int32_t value) {
        _cacheCapacityBytes = static_cast<std::size_t>(value) * 1024ull * 1024ull;
        updateStorageObservations();
        notifyOptionsChanged();
    });

    connect(&_tileCodecAction, &OptionAction::currentIndexChanged, this, [this] {
        updateZstdCompressionLevelAvailability();
        notifyOptionsChanged();
    });
    connect(&_zstdCompressionLevelAction, &IntegralAction::valueChanged, this, [this] { notifyOptionsChanged(); });

    connect(&_packedTilesAction, &ToggleAction::toggled, this, [this] {
        updatePackedTileControls();
        updateStorageObservations();
        notifyOptionsChanged();
    });

    connect(&_superTileRowsAction, &IntegralAction::valueChanged, this, [this] {
        updateStorageObservations();
        notifyOptionsChanged();
    });

    connect(&_superTileColumnsAction, &IntegralAction::valueChanged, this, [this] {
        updateStorageObservations();
        notifyOptionsChanged();
    });

    if (initializeFromConfiguration)
    {
        _cacheCapacityBytes = configuration.cacheCapacityBytes;
        const auto cacheLimitMiB = configuration.cacheCapacityBytes / (1024ull * 1024ull) + (configuration.cacheCapacityBytes % (1024ull * 1024ull) != 0);
        const QSignalBlocker cacheLimitBlocker(&_cacheCapacityBytesAction);
        _cacheCapacityBytesAction.setValue(static_cast<std::int32_t>(std::clamp<std::size_t>(cacheLimitMiB, 1, 65536)));
        _rowTileSizeAction.setValue(static_cast<std::int32_t>(configuration.rowTileSize));
        _columnTileSizeAction.setValue(static_cast<std::int32_t>(configuration.columnTileSize));
        _squareTilesAction.setChecked(configuration.rowTileSize == configuration.columnTileSize);
        _cacheCapacityAction.setValue(static_cast<std::int32_t>(configuration.cacheCapacity));
        _packedTilesAction.setChecked(configuration.packedTiles);
        _rowStripHeightAction.setValue(static_cast<std::int32_t>(configuration.rowStripHeight));
        _columnStripWidthAction.setValue(static_cast<std::int32_t>(configuration.columnStripWidth));
        _superTileRowsAction.setValue(static_cast<std::int32_t>(configuration.superTileRows));
        _superTileColumnsAction.setValue(static_cast<std::int32_t>(configuration.superTileColumns));
    }
    else
    {
        // Keep the previous balanced 1 MiB starting layout as an initial
        // suggestion, without exposing target-size or asymmetry controls.
        const auto initialGeometry = pointdata::storage::calculateTiledStorageGeometry(_rows, _columns, _elementSize, 1024ull * 1024ull, 50);
        _rowTileSizeAction.setValue(static_cast<std::int32_t>(initialGeometry.rowTileSize));
        _columnTileSizeAction.setValue(static_cast<std::int32_t>(initialGeometry.columnTileSize));
        updateTileSizePercentages();
        updateCacheCapacity();
    }

    updateStorageObservations();
    updateTileSizePercentages();
    updatePackedTileControls();
    updateZstdCompressionLevelAvailability();

    if (initializeFromConfiguration)
    {
        _initialConfiguration = getConfiguration();
        _hasInitialConfiguration = true;
    }
}

void TiledStorageBackendConfigurationAction::fromVariantMap(const QVariantMap& variantMap)
{
    StorageBackendConversionAction::fromVariantMap(variantMap);
    getSettingsGroupAction().setText("Layout settings");

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
            restoreAction(_rowStripHeightAction, performanceSettings);
            restoreAction(_columnStripWidthAction, performanceSettings);
        }

    }

    auto projectResourceStagingPolicyMap = variantMap.value(_projectResourceStagingPolicyAction.getSerializationName()).toMap();
    if (projectResourceStagingPolicyMap.contains("Value"))
        projectResourceStagingPolicyMap["Value"] = mv::util::normalizeProjectResourceStagingPolicyName(projectResourceStagingPolicyMap.value("Value").toString());
    if (projectResourceStagingPolicyMap.contains("Options"))
    {
        auto projectResourceStagingPolicyOptions = projectResourceStagingPolicyMap.value("Options").toStringList();
        for (auto& option : projectResourceStagingPolicyOptions)
            option = mv::util::normalizeProjectResourceStagingPolicyName(option);
        projectResourceStagingPolicyMap["Options"] = projectResourceStagingPolicyOptions;
    }
    if (!projectResourceStagingPolicyMap.isEmpty())
        _projectResourceStagingPolicyAction.fromVariantMap(projectResourceStagingPolicyMap);
    restoreAction(_zstdCompressionLevelAction, variantMap);
    updateProjectResourceStagingPolicyOption();
    // Accept value-only settings written by legacy presets.
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

    restoreLegacyIntegralValue(_rowTileSizeAction, "TileHeight", "RowTileSizeAction");
    restoreLegacyIntegralValue(_columnTileSizeAction, "TileWidth", "ColumnTileSizeAction");
    restoreLegacyToggleValue(_packedTilesAction, "PackedTiles", "PackedTilesAction");
    restoreLegacyIntegralValue(_rowStripHeightAction, "RowStripHeight", "RowStripHeightAction");
    restoreLegacyIntegralValue(_columnStripWidthAction, "ColumnStripWidth", "ColumnStripWidthAction");
    restoreLegacyIntegralValue(_superTileRowsAction, "SuperTileRows", "SuperTileRowsAction");
    restoreLegacyIntegralValue(_superTileColumnsAction, "SuperTileColumns", "SuperTileColumnsAction");
    restoreLegacyIntegralValue(_bucketingThreadCountAction, "BucketingThreadCount", "BucketingThreadCountAction");
    restoreLegacyIntegralValue(_stagingThreadCountAction, "StagingThreadCount", "StagingThreadCountAction");
    restoreLegacyIntegralValue(_rowChunkTileFactorAction, "RowChunkTileFactor", "RowChunkTileFactorAction");
    _squareTilesAction.setChecked(_rowTileSizeAction.getValue() == _columnTileSizeAction.getValue());

    updateTileSizePercentages();

    // Restore the explicit cache capacity after tile dimensions have been
    // restored, since geometry changes normally update its suggested value.
    restoreAction(_cacheCapacityAction, variantMap);
    restoreLegacyIntegralValue(_cacheCapacityAction, "CacheCapacity", "CacheCapacityAction");
    restoreAction(_cacheCapacityBytesAction, variantMap);
    restoreLegacyIntegralValue(_cacheCapacityBytesAction, "CacheCapacityBytesMiB", "CacheCapacityBytesAction");
    _cacheCapacityBytes = static_cast<std::size_t>(_cacheCapacityBytesAction.getValue()) * 1024ull * 1024ull;
    restoreAction(_tileCodecAction, variantMap);
    restoreAction(_importBenchmarkEnabledAction, variantMap);
    restoreAction(_importBenchmarkDirectoryAction, variantMap);
    _importBenchmarkDirectoryAction.setEnabled(_importBenchmarkEnabledAction.isChecked());

    updatePackedTileControls();
    updateImportPerformanceSummary();
    updateZstdCompressionLevelAvailability();
    updateStorageObservations();
    notifyOptionsChanged();
}

QVariantMap TiledStorageBackendConfigurationAction::toVariantMap() const
{
    auto variantMap = StorageBackendConversionAction::toVariantMap();

    _cacheCapacityAction.insertIntoVariantMap(variantMap);
    _cacheCapacityBytesAction.insertIntoVariantMap(variantMap);
    if (_showImportPerformanceOptions)
    {
        _importBenchmarkEnabledAction.insertIntoVariantMap(variantMap);
        _importBenchmarkDirectoryAction.insertIntoVariantMap(variantMap);
    }
    _tileCodecAction.insertIntoVariantMap(variantMap);
    _zstdCompressionLevelAction.insertIntoVariantMap(variantMap);
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

    if (_showImportPerformanceOptions || _showConversionPerformanceOptions)
    {
        auto performanceSettings = _importPerformanceSettingsAction.toVariantMap();
        if (_showImportPerformanceOptions)
            _bucketingThreadCountAction.insertIntoVariantMap(performanceSettings);
        _stagingThreadCountAction.insertIntoVariantMap(performanceSettings);
        _rowChunkTileFactorAction.insertIntoVariantMap(performanceSettings);
        if (_showImportPerformanceOptions)
        {
            _rowStripHeightAction.insertIntoVariantMap(performanceSettings);
            _columnStripWidthAction.insertIntoVariantMap(performanceSettings);
        }

        auto importPerformance = _importPerformanceAction.toVariantMap();
        _importPerformanceSummaryAction.insertIntoVariantMap(importPerformance);
        importPerformance.insert(_importPerformanceSettingsAction.getSerializationName(), performanceSettings);
        variantMap.insert(_importPerformanceAction.getSerializationName(), importPerformance);
        if (_showImportPerformanceOptions)
            _projectResourceStagingPolicyAction.insertIntoVariantMap(variantMap);
    }

    return variantMap;
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

    if (getProjectResourceStagingPolicy() != _initialProjectResourceStagingPolicy)
        return true;

    if (!_hasInitialConfiguration)
        return false;

    const auto configuration = getConfiguration();
    return configuration.rowTileSize != _initialConfiguration.rowTileSize ||
        configuration.columnTileSize != _initialConfiguration.columnTileSize ||
        configuration.cacheCapacity != _initialConfiguration.cacheCapacity ||
        configuration.cacheCapacityBytes != _initialConfiguration.cacheCapacityBytes ||
        configuration.tileCodecName != _initialConfiguration.tileCodecName ||
        configuration.tileCompressionLevel != _initialConfiguration.tileCompressionLevel ||
        configuration.packedTiles != _initialConfiguration.packedTiles ||
        configuration.superTileRows != _initialConfiguration.superTileRows ||
        configuration.superTileColumns != _initialConfiguration.superTileColumns ||
        configuration.rowStripHeight != _initialConfiguration.rowStripHeight ||
        configuration.columnStripWidth != _initialConfiguration.columnStripWidth ||
        (_showConversionPerformanceOptions && (configuration.conversionWorkerCount != _initialConfiguration.conversionWorkerCount || configuration.conversionChunkTileRows != _initialConfiguration.conversionChunkTileRows));
}

pointdata::storage::StorageBackendConfiguration TiledStorageBackendConfigurationAction::getConfiguration() const
{
    pointdata::storage::StorageBackendConfiguration configuration;
    configuration.rowTileSize          = static_cast<std::size_t>(_rowTileSizeAction.getValue());
    configuration.columnTileSize       = static_cast<std::size_t>(_columnTileSizeAction.getValue());
    configuration.cacheCapacity         = static_cast<std::size_t>(_cacheCapacityAction.getValue());
    configuration.cacheCapacityBytes = _cacheCapacityBytes;
    configuration.tileCodecName         = mv::util::BlobCodec::typeToString(mv::util::codecRegistry().typeFromDisplayName(_tileCodecAction.getCurrentText()));
    configuration.tileCompressionLevel  = _zstdCompressionLevelAction.getValue();
    configuration.packedTiles           = _packedTilesAction.isChecked();
    configuration.superTileRows         = static_cast<std::size_t>(_superTileRowsAction.getValue());
    configuration.superTileColumns      = static_cast<std::size_t>(_superTileColumnsAction.getValue());
    configuration.rowStripHeight        = static_cast<std::size_t>(_rowStripHeightAction.getValue());
    configuration.columnStripWidth      = static_cast<std::size_t>(_columnStripWidthAction.getValue());
    if (_showConversionPerformanceOptions)
    {
        configuration.conversionWorkerCount = static_cast<std::size_t>(_stagingThreadCountAction.getValue());
        configuration.conversionChunkTileRows = static_cast<std::size_t>(_rowChunkTileFactorAction.getValue());
    }
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

void TiledStorageBackendConfigurationAction::enableConversionPerformanceOptions(const std::size_t workerCount, const std::size_t chunkTileRows)
{
    if (_showConversionPerformanceOptions)
        return;

    _showConversionPerformanceOptions = true;
    _importPerformanceAction.setText("Conversion performance");
    _performanceOptionsGroupAction.setDescription(
        "<p>Controls the worker count and tile-row chunk size used during conversion. More workers may improve throughput but use more CPU and memory.</p>");
    _stagingThreadCountAction.setText("Worker count");
    _rowChunkTileFactorAction.setText("Chunk tile-height factor");
    _stagingThreadCountAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _rowChunkTileFactorAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _stagingThreadCountAction.setValue(static_cast<std::int32_t>(std::clamp<std::size_t>(workerCount, 1, 32)));
    _rowChunkTileFactorAction.setValue(static_cast<std::int32_t>(std::clamp<std::size_t>(chunkTileRows, 1, 256)));
    _importPerformanceSummaryAction.setDefaultWidgetFlags(StringAction::LineEdit);
    _importPerformanceSummaryAction.setEnabled(false);
    _importPerformanceSummaryAction.setStretch(1);
    _importPerformanceSettingsAction.setShowLabels(true);
    _importPerformanceSettingsAction.setLabelSizingType(GroupAction::LabelSizingType::Fixed);
    _stagingThreadCountAction.setDescription("<p>Maximum worker threads used to generate converted tiles. More workers can improve throughput but use more CPU and memory.</p>");
    _rowChunkTileFactorAction.setDescription("<p>Requested number of tile heights grouped into each conversion chunk. Chunks may expand to provide enough packed supertiles for the selected workers; larger chunks need more temporary memory.</p>");
    _importPerformanceAction.addAction(&_importPerformanceSummaryAction);
    _importPerformanceAction.addAction(&_importPerformanceSettingsAction);
    _importPerformanceSettingsAction.addAction(&_stagingThreadCountAction);
    _importPerformanceSettingsAction.addAction(&_rowChunkTileFactorAction);
    _performanceOptionsGroupAction.addAction(&_stagingThreadCountAction);
    _performanceOptionsGroupAction.addAction(&_rowChunkTileFactorAction);
    setGroupActionVisibility(
        &_performanceOptionsGroupAction,
        mv::settings().getDeveloperSettingsAction().getEnableDeveloperAddonsAction().isChecked());

    updateImportPerformanceSummary();
    connect(&_stagingThreadCountAction, &IntegralAction::valueChanged, this, [this] {
        updateImportPerformanceSummary();
        notifyOptionsChanged();
    });
    connect(&_rowChunkTileFactorAction, &IntegralAction::valueChanged, this, [this] {
        updateImportPerformanceSummary();
        notifyOptionsChanged();
    });
}

mv::util::ProjectResourceStagingPolicy TiledStorageBackendConfigurationAction::getProjectResourceStagingPolicy() const
{
    const auto currentText = _projectResourceStagingPolicyAction.getCurrentText();
    return currentText.startsWith(QStringLiteral("Inherited ("))
        ? mv::util::ProjectResourceStagingPolicy::Inherit
        : mv::util::projectResourceStagingPolicyFromName(currentText);
}

int TiledStorageBackendConfigurationAction::getZstdCompressionLevel() const
{
    return _zstdCompressionLevelAction.getValue();
}

QString TiledStorageBackendConfigurationAction::getImportBenchmarkFilePath() const
{
    if (!_showImportPerformanceOptions || !_importBenchmarkEnabledAction.isChecked() || !_importBenchmarkDirectoryAction.isValid())
        return QString();

    const auto timestamp = QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd-HHmmss"));
    return QDir(_importBenchmarkDirectoryAction.getDirectory()).filePath(QStringLiteral("h5ad-import-benchmark-%1.json").arg(timestamp));
}

QString TiledStorageBackendConfigurationAction::getImportBenchmarkDirectory() const
{
    return _importBenchmarkDirectoryAction.getDirectory();
}

void TiledStorageBackendConfigurationAction::synchronizeSquareTileSize(const bool rowTileSizeChanged)
{
    if (_updatingSquareTileSize)
        return;

    if (_squareTilesAction.isChecked())
    {
        const auto tileSize = rowTileSizeChanged ? _rowTileSizeAction.getValue() : _columnTileSizeAction.getValue();
        auto& otherTileSizeAction = rowTileSizeChanged ? _columnTileSizeAction : _rowTileSizeAction;
        if (otherTileSizeAction.getValue() != tileSize)
        {
            _updatingSquareTileSize = true;
            otherTileSizeAction.setValue(tileSize);
            _updatingSquareTileSize = false;
        }
    }

    updateTileSizePercentages();
    updateCacheCapacity();
    notifyOptionsChanged();
}

void TiledStorageBackendConfigurationAction::updateProjectResourceStagingPolicyOption()
{
    const auto inheritedPolicy = mv::projects().resolveProjectResourceStagingPolicy(mv::util::ProjectResourceStagingPolicy::Inherit);

    auto options = _projectResourceStagingPolicyAction.getOptions();
    if (options.isEmpty())
        return;

    const auto selectedPolicy = getProjectResourceStagingPolicy();
    options[0] = QStringLiteral("Inherited (%1)").arg(mv::util::projectResourceStagingPolicyName(inheritedPolicy));

    const QSignalBlocker blocker(&_projectResourceStagingPolicyAction);
    _projectResourceStagingPolicyAction.setOptions(options);
    _projectResourceStagingPolicyAction.setCurrentText(selectedPolicy == mv::util::ProjectResourceStagingPolicy::Inherit
        ? options[0]
        : mv::util::projectResourceStagingPolicyName(selectedPolicy));
}

void TiledStorageBackendConfigurationAction::updateImportPerformanceSummary()
{
    if (_showConversionPerformanceOptions)
        _importPerformanceSummaryAction.setString(QStringLiteral("%1 workers, chunk tile-height factor %2")
            .arg(_stagingThreadCountAction.getValue())
            .arg(_rowChunkTileFactorAction.getValue()));
    else
        _importPerformanceSummaryAction.setString(QStringLiteral("%1 bucket threads, %2 staging threads, chunk tile-height factor %3; %4x%5 row/column strips")
            .arg(_bucketingThreadCountAction.getValue())
            .arg(_stagingThreadCountAction.getValue())
            .arg(_rowChunkTileFactorAction.getValue())
            .arg(_rowStripHeightAction.getValue())
            .arg(_columnStripWidthAction.getValue()));
}

void TiledStorageBackendConfigurationAction::updatePackedTileControls()
{
    _superTileRowsAction.setEnabled(_packedTilesAction.isChecked());
    _superTileColumnsAction.setEnabled(_packedTilesAction.isChecked());
    _tileCodecAction.setEnabled(_packedTilesAction.isChecked());
    _projectResourceStagingPolicyAction.setEnabled(!_showImportPerformanceOptions || _packedTilesAction.isChecked());
}

void TiledStorageBackendConfigurationAction::updateZstdCompressionLevelAvailability()
{
    const auto codecName = mv::util::BlobCodec::typeToString(mv::util::codecRegistry().typeFromDisplayName(_tileCodecAction.getCurrentText()));
    _zstdCompressionLevelAction.setEnabled(codecName == QStringLiteral("zstd"));
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
        updateTileCountAction(0, false);
        for (auto* action : { &_superTileCountAction, &_tileByteSizeAction, &_superTileByteSizeAction, &_cacheByteSizeAction })
            action->setString(tr("Unavailable"));
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
    updateTileCountAction(memoryEstimate.tileCount);
    const auto tileRows = _rows / static_cast<std::uint64_t>(_rowTileSizeAction.getValue()) + (_rows % static_cast<std::uint64_t>(_rowTileSizeAction.getValue()) != 0);
    const auto tileColumns = _columns / static_cast<std::uint64_t>(_columnTileSizeAction.getValue()) + (_columns % static_cast<std::uint64_t>(_columnTileSizeAction.getValue()) != 0);
    const auto superTileRows = static_cast<std::uint64_t>(_superTileRowsAction.getValue());
    const auto superTileColumns = static_cast<std::uint64_t>(_superTileColumnsAction.getValue());
    const auto superTileRowCount = tileRows / superTileRows + (tileRows % superTileRows != 0);
    const auto superTileColumnCount = tileColumns / superTileColumns + (tileColumns % superTileColumns != 0);
    const auto superTileCount = _packedTilesAction.isChecked() ? superTileRowCount * superTileColumnCount : memoryEstimate.tileCount;
    const auto tilesPerSuperTile = _packedTilesAction.isChecked()
        ? std::min(tileRows, superTileRows) * std::min(tileColumns, superTileColumns)
        : 1;
    const auto largestSuperTileBytes = memoryEstimate.largestTileBytes * tilesPerSuperTile;
    updateEstimateAction(_superTileCountAction, superTileCount, tr("supertiles"), 10000,
        tr("High supertile count"), tr("A high supertile count increases container and metadata overhead."));
    updateEstimateAction(_tileByteSizeAction, memoryEstimate.largestTileBytes, QString(), 100ull * 1024ull * 1024ull,
        tr("Large tile"), tr("Tiles above 100 MiB can use substantial memory per read and may not fit in the configured cache."));
    updateEstimateAction(_superTileByteSizeAction, largestSuperTileBytes, QString(), 1024ull * 1024ull * 1024ull,
        tr("Large supertile"), tr("Supertile payloads above 1 GiB can increase read amplification and temporary memory requirements."));
    updateEstimateAction(_cacheByteSizeAction, memoryEstimate.cachedTileBytes, QString(), 4ull * 1024ull * 1024ull * 1024ull,
        tr("Large cache estimate"), tr("A cache estimate above 4 GiB can put substantial pressure on system memory."));
    updateStagingSpaceObservation();
}

void TiledStorageBackendConfigurationAction::updateEstimateAction(StringAction& action, const std::uint64_t value, const QString& unit, const std::uint64_t warningThreshold, const QString& warningTitle, const QString& warningDescription)
{
    action.setString(unit.isEmpty() ? util::getNoBytesHumanReadable(value) : QStringLiteral("%L1 %2").arg(static_cast<qulonglong>(value)).arg(unit));
    auto& warningAction = action.getTrailingAction();
    const auto warning = value > warningThreshold;
    if (warning)
    {
        warningAction.setIcon(mv::util::StyledIcon(QStringLiteral("triangle-exclamation")).withColor(QApplication::palette().color(QPalette::Disabled, QPalette::Text)));
        const auto title = warningTitle.toHtmlEscaped();
        const auto description = warningDescription.toHtmlEscaped();
        warningAction.setToolTip(QStringLiteral("<qt><table width=\"420\" cellpadding=\"10\"><tr><td><b>%1</b><br/>%2</td></tr></table></qt>").arg(title, description));
    }
    else
        warningAction.setToolTip(QString());
    warningAction.setVisible(warning);
}

void TiledStorageBackendConfigurationAction::updateTileCountAction(const std::uint64_t tileCount, const bool available)
{
    if (!available)
    {
        _tileCountAction.setString(tr("Unavailable"));
        _tileCountAction.getTrailingAction().setToolTip(QString());
        _tileCountAction.getTrailingAction().setVisible(false);
        return;
    }

    _tileCountAction.setString(QStringLiteral("%L1 tiles").arg(static_cast<qulonglong>(tileCount)));

    auto& warningAction = _tileCountAction.getTrailingAction();
    const auto warning = tileCount > 10000;
    if (warning)
    {
        warningAction.setIcon(mv::util::StyledIcon(QStringLiteral("triangle-exclamation")).withColor(QApplication::palette().color(QPalette::Disabled, QPalette::Text)));

        const auto headingColor = QApplication::palette().color(QPalette::Link).name();
        const auto heading = tr("High tile count").toHtmlEscaped();
        const auto description = tr("This layout creates %L1 tiles. A high tile count can increase index memory and import time; consider increasing tile height or width.")
            .arg(static_cast<qulonglong>(tileCount))
            .toHtmlEscaped();
        warningAction.setToolTip(QStringLiteral(
            "<qt><table width=\"440\" cellpadding=\"12\" cellspacing=\"0\" border=\"0\"><tr><td>"
            "<div style=\"font-size: 11pt; font-weight: bold; color: %1; margin-bottom: 10px;\">%2</div>"
            "<div style=\"font-size: 9pt;\">%3</div>"
            "</td></tr></table></qt>").arg(headingColor, heading, description));
    }
    else
    {
        warningAction.setToolTip(QString());
    }
    warningAction.setVisible(warning);
}

void TiledStorageBackendConfigurationAction::updateStagingSpaceObservation()
{
    if (!_showImportStagingSpaceObservation)
        return;

    _diskSpaceAction.setToolTip(tr("App-managed staging directory: %1").arg(_stagingDirectory));

    if (_rows == 0 || _columns == 0 || _elementSize == 0 ||
        _rows > std::numeric_limits<std::uint64_t>::max() / _columns ||
        _rows * _columns > std::numeric_limits<std::uint64_t>::max() / _elementSize)
    {
        _diskSpaceAction.setString(tr("Unavailable"));
        _diskSpaceAction.getTrailingAction().setToolTip(tr("The required staging space could not be estimated. Path: %1").arg(_stagingDirectory));
        _diskSpaceAction.getTrailingAction().setVisible(true);
        return;
    }

    const auto maximum = std::numeric_limits<std::uint64_t>::max();
    const auto elementSize = static_cast<std::uint64_t>(_elementSize);
    const auto matrixBytes = (_columns != 0 && _rows > maximum / _columns) ||
        (_rows * _columns != 0 && elementSize > maximum / (_rows * _columns))
        ? maximum
        : _rows * _columns * elementSize;
    const auto stagingBytes = matrixBytes > maximum / 2 ? maximum : matrixBytes * 2;
    const auto stagingSize = util::getNoBytesHumanReadable(stagingBytes);
    _diskSpaceAction.getTrailingAction().setIcon(mv::util::StyledIcon(QStringLiteral("triangle-exclamation")).withColor(QApplication::palette().color(QPalette::Disabled, QPalette::Text)));
    _diskSpaceAction.getTrailingAction().setToolTip(tr("Free space could not be checked. Staging path: %1").arg(_stagingDirectory.toHtmlEscaped()));
    QStorageInfo stagingStorage(_stagingDirectory);
    stagingStorage.refresh();
    if (!stagingStorage.isValid() || !stagingStorage.isReady() || stagingStorage.bytesAvailable() < 0)
    {
        _diskSpaceAction.setString(tr("Unavailable (requires %1)").arg(stagingSize));
        _diskSpaceAction.getTrailingAction().setVisible(true);
        return;
    }

    const auto availableBytes = static_cast<std::uint64_t>(stagingStorage.bytesAvailable());
    const auto availableSpace = util::getNoBytesHumanReadable(availableBytes);
    _diskSpaceAction.setString(tr("%1 available / %2 estimated").arg(availableSpace, stagingSize));
    _diskSpaceAction.getTrailingAction().setToolTip(tr("Staging path: %1. Estimate includes both row-strip and column-strip payloads at their uncompressed size. Compression may reduce actual use; packed-file metadata adds a small amount. Changing the application temporary directory takes effect after restart.")
        .arg(_stagingDirectory.toHtmlEscaped()));
    if (availableBytes < stagingBytes)
    {
        _diskSpaceAction.getTrailingAction().setIcon(mv::util::StyledIcon(QStringLiteral("triangle-exclamation")).withColor(QApplication::palette().color(QPalette::Disabled, QPalette::Text)));
        _diskSpaceAction.getTrailingAction().setVisible(true);
    }
    else
    {
        _diskSpaceAction.getTrailingAction().setToolTip(tr("Staging path: %1. Dense-equivalent estimate; sparse data and compression may reduce actual use.")
            .arg(_stagingDirectory.toHtmlEscaped()));
        _diskSpaceAction.getTrailingAction().setVisible(false);
    }
}

void TiledStorageBackendConfigurationAction::estimateBalancedLayout()
{
    if (_rows == 0 || _columns == 0 || _elementSize == 0)
        return;

    constexpr std::uint64_t minimumTileBytes = 1024ull * 1024ull;
    constexpr std::uint64_t maximumTileBytes = 16ull * 1024ull * 1024ull;
    constexpr std::uint64_t targetTileCount = 50000;
    constexpr std::uint64_t targetTilesPerSuperTile = 16;
    constexpr std::uint64_t maximumSuperTileColumns = 4;

    const auto totalBytes = static_cast<long double>(_rows) * static_cast<long double>(_columns) * static_cast<long double>(_elementSize);
    const auto bytesPerTile = static_cast<std::uint64_t>(std::clamp(totalBytes / targetTileCount, static_cast<long double>(minimumTileBytes), static_cast<long double>(maximumTileBytes)));
    const auto estimatedSquareSide = static_cast<std::uint64_t>(std::sqrt(static_cast<long double>(bytesPerTile) / static_cast<long double>(_elementSize)));
    const auto tileDimension = std::clamp<std::uint64_t>(estimatedSquareSide, 1, std::min<std::uint64_t>({ _rows, _columns, static_cast<std::uint64_t>(std::numeric_limits<std::int32_t>::max()) }));
    const auto tileRows = _rows / tileDimension + (_rows % tileDimension != 0);
    const auto tileColumns = _columns / tileDimension + (_columns % tileDimension != 0);
    const auto superTileColumns = std::min(tileColumns, maximumSuperTileColumns);
    const auto superTileRows = std::min(tileRows, std::max<std::uint64_t>(1, targetTilesPerSuperTile / superTileColumns));

    _squareTilesAction.setChecked(true);
    _rowTileSizeAction.setValue(static_cast<std::int32_t>(tileDimension));
    _columnTileSizeAction.setValue(static_cast<std::int32_t>(tileDimension));
    _packedTilesAction.setChecked(true);
    _superTileRowsAction.setValue(static_cast<std::int32_t>(superTileRows));
    _superTileColumnsAction.setValue(static_cast<std::int32_t>(superTileColumns));

    if (_showImportPerformanceOptions)
    {
        const auto detectedThreadCount = std::thread::hardware_concurrency();
        const auto suggestedWorkerCount = std::clamp<std::size_t>(detectedThreadCount > 0 ? detectedThreadCount : 8, 1, 8);
        _bucketingThreadCountAction.setValue(static_cast<std::int32_t>(suggestedWorkerCount));
        _stagingThreadCountAction.setValue(static_cast<std::int32_t>(suggestedWorkerCount));
        _rowChunkTileFactorAction.setValue(2);
    }

    updatePackedTileControls();
    updateTileSizePercentages();
    updateCacheCapacity();
    updateStorageObservations();
    notifyOptionsChanged();
}
}
