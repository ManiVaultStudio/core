// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "../interface/StorageBackendConversionAction.h"
#include "TiledStorageGeometry.h"

#include <actions/DirectoryPickerAction.h>
#include <actions/DecimalAction.h>
#include <actions/HorizontalGroupAction.h>
#include <actions/IntegralAction.h>
#include <actions/OptionAction.h>
#include <actions/StringAction.h>
#include <actions/ToggleAction.h>
#include <actions/TriggerAction.h>
#include <actions/VerticalGroupAction.h>
#include <QString>

namespace mv::gui
{

/**
 * @brief Provides controls for configuring tiled storage.
 *
 * The tiled backend owns these settings because tile geometry and cache size
 * describe its physical representation. The controls can be used during import
 * as well as by the conversion-specific action specialization. Live observations
 * report estimated tile-index size and whether a tile exceeds the cache byte
 * limit. Pre-flight rows estimate tile, supertile, cache, and staging costs;
 * their warning thresholds are planning heuristics, not conversion failures.
 *
 * @author T. Kroes
 */
class POINTDATA_EXPORT TiledStorageBackendConfigurationAction : public StorageBackendConversionAction
{
    Q_OBJECT

public:
    /**
     * @brief Constructs controls for configuring tiled storage for a matrix.
     *
     * Tile height and width remain directly editable. When requested,
     * the current physical geometry, cache limits, and packed tile codec are
     * copied from \p configuration for backend inspection or reconfiguration.
     * @param parent Parent action that owns this configuration action.
     * @param rows Logical matrix row count used to bound tile geometry.
     * @param columns Logical matrix column count used to bound tile geometry.
     * @param elementSize Size of one matrix element in bytes.
     * @param configuration Existing tiled settings to display when initializing from configuration.
     * @param initializeFromConfiguration Whether to display existing geometry and cache values instead of suggestions.
     * @param currentElementTypeIndex Current matrix element type; when supplied, the action exposes a target-type selector.
     */
    TiledStorageBackendConfigurationAction(QObject* parent, std::uint64_t rows, std::uint64_t columns, std::size_t elementSize, const pointdata::storage::StorageBackendConfiguration& configuration = {}, bool initializeFromConfiguration = false, std::optional<std::uint32_t> currentElementTypeIndex = {}, bool showImportStagingSpaceObservation = false, const QString& stagingDirectory = QString());

    /** Returns the tiled backend type represented by this action. */
    pointdata::storage::StorageBackendType getTargetBackendType() const override;

    /** Returns the selected tile geometry, cache limits, and packed tile codec. */
    pointdata::storage::StorageBackendConfigurationOptionsPtr getConversionOptions() const override;

    /** Returns whether the selected element type or tiled settings differ from their initial values. */
    bool hasChanges() const override;

    /** Restores the tiled backend controls from a preset map. */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /** Serializes the tiled backend controls for presets. */
    QVariantMap toVariantMap() const override;

    /** Returns the current tiled-storage configuration. */
    pointdata::storage::StorageBackendConfiguration getConfiguration() const;

    /** Returns the optional HDF5 import bucketing worker limit. */
    [[nodiscard]] std::size_t getBucketingThreadCount() const;

    /** Returns the optional HDF5 import staging worker limit. */
    [[nodiscard]] std::size_t getStagingThreadCount() const;

    /** Returns the optional CSR row-chunk tile-height factor. */
    [[nodiscard]] std::size_t getRowChunkTileFactor() const;

    /** Enables bounded parallel generation controls for backend conversion. */
    void enableConversionPerformanceOptions(std::size_t workerCount, std::size_t chunkTileRows);

    /** Returns the Zstandard compression level selected for H5AD import staging. */
    [[nodiscard]] int getZstdCompressionLevel() const;

    /** Returns the optional HDF5 import benchmark report path selected in developer options. */
    [[nodiscard]] QString getImportBenchmarkFilePath() const;

    /** Returns the directory selected for the next HDF5 import benchmark report. */
    [[nodiscard]] QString getImportBenchmarkDirectory() const;

private:
    void estimateBalancedLayout();
    void updateTileSizePercentages();
    void updateTileCountAction(std::uint64_t tileCount, bool available = true);
    void setRowTileSizeFromPercentage();
    void setColumnTileSizeFromPercentage();
    void updateCacheCapacity();
    void updateStorageObservations();
    void updatePackedTileControls();
    void updateImportPerformanceSummary();
    void updateZstdCompressionLevelAvailability();
    void updateStagingSpaceObservation();
    void updateEstimateAction(StringAction& action, std::uint64_t value, const QString& unit, std::uint64_t warningThreshold, const QString& warningTitle, const QString& warningDescription);
    void synchronizeSquareTileSize(bool rowTileSizeChanged);

    std::uint64_t _rows               = 0;                                                 /**< Logical matrix row count used to bound geometry and observations. */
    std::uint64_t _columns            = 0;                                                 /**< Logical matrix column count used to bound geometry and observations. */
    std::size_t   _elementSize        = 0;                                                 /**< Current target element size in bytes. */
    std::size_t   _cacheCapacityBytes = pointdata::storage::defaultTileCacheCapacityBytes; /**< Maximum decoded tile bytes permitted in cache. */
    pointdata::storage::StorageBackendConfiguration _initialConfiguration;                 /**< Original tiled settings used to detect changes in configure mode. */
    bool _hasInitialConfiguration = false;                                                 /**< Whether an original tiled configuration was provided. */

    VerticalGroupAction         _developerOptionsGroupAction;          /**< Developer-only settings, shown when the global developer option is enabled. */
    VerticalGroupAction         _performanceOptionsGroupAction;        /**< Developer-only worker and chunk tuning controls. */
    VerticalGroupAction         _compressionSettingsAction;            /**< Codec and compression-level settings. */
    VerticalGroupAction         _superTileOptionsAction;               /**< Packed-tile and supertile geometry settings. */
    VerticalGroupAction         _stripLayoutOptionsAction;             /**< Row and column strip dimensions. */
    TriggerAction               _estimateBalancedLayoutAction;        /**< Applies a balanced tile and supertile layout estimate. */
    HorizontalGroupAction       _rowTileSizeGroupAction;             /**< Row tile size and percentage controls. */
    ToggleAction                _squareTilesAction;                  /**< Keeps row and column tile dimensions equal. */
    IntegralAction              _rowTileSizeAction;                  /**< Maximum number of matrix rows per tile. */
    DecimalAction               _rowTileSizePercentageAction;        /**< Row tile height as a percentage of matrix rows. */
    HorizontalGroupAction       _columnTileSizeGroupAction;          /**< Column tile size and percentage controls. */
    IntegralAction              _columnTileSizeAction;               /**< Maximum number of matrix columns per tile. */
    DecimalAction               _columnTileSizePercentageAction;     /**< Column tile width as a percentage of matrix columns. */
    StringAction                _tileCountAction;                    /**< Read-only current tile count with an optional warning indicator. */
    StringAction                _superTileCountAction;               /**< Read-only estimated number of packed supertiles. */
    StringAction                _tileByteSizeAction;                 /**< Estimated uncompressed payload size of one tile. */
    StringAction                _superTileByteSizeAction;            /**< Estimated uncompressed payload size of one supertile. */
    StringAction                _cacheByteSizeAction;               /**< Estimated decoded cache use under the configured limits. */
    StringAction                _diskSpaceAction;                    /**< Available staging disk space compared with dense-equivalent data size. */
    IntegralAction              _cacheCapacityAction;                /**< Maximum resident tile count. */
    IntegralAction              _cacheCapacityBytesAction;           /**< Maximum decoded tile-cache bytes, in MiB. */
    OptionAction                _tileCodecAction;                    /**< Codec used for independently encoded packed tile payloads. */
    ToggleAction                _packedTilesAction;                  /**< Whether serialized tile blocks are grouped into super tiles. */
    IntegralAction              _rowStripHeightAction;                /**< Number of rows in each compressed row strip. */
    IntegralAction              _columnStripWidthAction;              /**< Number of columns in each compressed column strip. */
    IntegralAction              _superTileRowsAction;                /**< Base tile rows per serialized super tile. */
    IntegralAction              _superTileColumnsAction;             /**< Base tile columns per serialized super tile. */
    HorizontalGroupAction       _importPerformanceAction;            /**< Collapsible import or conversion performance settings. */
    StringAction                _importPerformanceSummaryAction;     /**< Read-only summary of active performance settings. */
    VerticalGroupAction         _importPerformanceSettingsAction;    /**< Group containing worker and chunk settings. */
    IntegralAction              _bucketingThreadCountAction;         /**< Maximum workers used to bucket HDF5 input. */
    IntegralAction              _stagingThreadCountAction;           /**< Maximum workers used for staging or tile conversion. */
    IntegralAction              _rowChunkTileFactorAction;           /**< Requested CSR tile-row factor per conversion chunk. */
    IntegralAction              _zstdCompressionLevelAction;         /**< H5AD staging compression level when Zstandard is selected. */
    ToggleAction                _importBenchmarkEnabledAction;       /**< Whether to write an HDF5 import benchmark report. */
    DirectoryPickerAction       _importBenchmarkDirectoryAction;     /**< Directory for the optional HDF5 import benchmark report. */
    QString                     _stagingDirectory;                          /**< App-managed directory checked for import staging space. */
    bool                        _showImportPerformanceOptions = false;      /**< Whether the action exposes import worker settings. */
    bool                        _showConversionPerformanceOptions = false;  /**< Whether the performance controls are configured for conversion. */
    bool                        _showImportStagingSpaceObservation = false; /**< Whether to display the import disk-space estimate. */
    bool                        _updatingTileSizePercentage = false;        /**< Guards reciprocal updates between size and percentage controls. */
    bool                        _updatingSquareTileSize = false;            /**< Guards reciprocal tile size updates when square tiles are enabled. */
};

}
