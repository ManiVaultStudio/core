// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "../interface/StorageBackendConversionAction.h"
#include "TiledStorageAsymmetryAction.h"
#include "TiledStorageGeometry.h"

#include <actions/MessageCardAction.h>
#include <actions/DecimalAction.h>
#include <actions/HorizontalGroupAction.h>
#include <actions/IntegralAction.h>
#include <actions/OptionAction.h>
#include <actions/StringAction.h>
#include <actions/ToggleAction.h>
#include <actions/VerticalGroupAction.h>
#include <util/ProjectResourceStagingPolicy.h>
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
 * limit. The high tile-count warning begins at 100,000 descriptors and is a
 * heuristic, not a conversion failure.
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
     * The tile-size and asymmetry controls calculate a suggested geometry;
     * the row and column dimensions remain directly editable. When requested,
     * the current physical geometry and cache capacity are copied from
     * \p configuration for backend inspection or reconfiguration.
     * @param parent Parent action that owns this configuration action.
     * @param rows Logical matrix row count used to bound tile geometry.
     * @param columns Logical matrix column count used to bound tile geometry.
     * @param elementSize Size of one matrix element in bytes.
     * @param configuration Existing tiled settings to display when initializing from configuration.
     * @param initializeFromConfiguration Whether to display existing geometry and cache values instead of suggestions.
     * @param currentElementTypeIndex Current matrix element type; when supplied, the action exposes a target-type selector.
     */
    TiledStorageBackendConfigurationAction(QObject* parent, std::uint64_t rows, std::uint64_t columns, std::size_t elementSize, const pointdata::storage::StorageBackendConfiguration& configuration = {}, bool initializeFromConfiguration = false, std::optional<std::uint32_t> currentElementTypeIndex = {}, bool showImportPerformanceOptions = false, std::size_t bucketingThreadCount = 4, std::size_t stagingThreadCount = 4, std::size_t rowChunkTileFactor = 3, bool showImportStagingSpaceObservation = false, const QString& stagingDirectory = QString(), mv::util::ProjectResourceStagingPolicy projectResourceStagingPolicy = mv::util::ProjectResourceStagingPolicy::Inherit);

    /** Returns the tiled backend type represented by this action. */
    pointdata::storage::StorageBackendType getTargetBackendType() const override;

    /** Returns the currently selected physical tile geometry and cache capacity. */
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

    /** Returns the project resource staging policy selected in an import configuration. */
    [[nodiscard]] mv::util::ProjectResourceStagingPolicy getProjectResourceStagingPolicy() const;

private:
    void updateProjectResourceStagingPolicyOption();
    void updateTileGeometryFromPolicy();
    void updateTargetTileSizeFromGeometry();
    void updateTileSizePercentages();
    void setRowTileSizeFromPercentage();
    void setColumnTileSizeFromPercentage();
    void updateTileSizingMode();
    void updateCacheCapacity();
    void updateStorageObservations();
    void updatePackedTileControls();
    void updateImportPerformanceSummary();
    void updateStagingSpaceObservation();

    std::uint64_t _rows               = 0;                                                 /**< Logical matrix row count used to bound geometry and observations. */
    std::uint64_t _columns            = 0;                                                 /**< Logical matrix column count used to bound geometry and observations. */
    std::size_t   _elementSize        = 0;                                                 /**< Current target element size in bytes. */
    std::size_t   _cacheCapacityBytes = pointdata::storage::defaultTileCacheCapacityBytes; /**< Maximum decoded tile bytes permitted in cache. */
    pointdata::storage::StorageBackendConfiguration _initialConfiguration;                 /**< Original tiled settings used to detect changes in configure mode. */
    bool _hasInitialConfiguration = false;                                                 /**< Whether an original tiled configuration was provided. */

    TiledStorageAsymmetryAction _tileAsymmetryAction;                /**< Row-versus-column tile preference control. */
    ToggleAction                _deriveTileDimensionsAction;         /**< Whether target size and asymmetry derive editable dimensions. */
    IntegralAction              _tileSizeAction;                     /**< Desired target tile payload size in MiB. */
    HorizontalGroupAction       _rowTileSizeGroupAction;             /**< Row tile size and percentage controls. */
    IntegralAction              _rowTileSizeAction;                  /**< Maximum number of matrix rows per tile. */
    DecimalAction               _rowTileSizePercentageAction;        /**< Row tile height as a percentage of matrix rows. */
    HorizontalGroupAction       _columnTileSizeGroupAction;          /**< Column tile size and percentage controls. */
    IntegralAction              _columnTileSizeAction;               /**< Maximum number of matrix columns per tile. */
    DecimalAction               _columnTileSizePercentageAction;     /**< Column tile width as a percentage of matrix columns. */
    IntegralAction              _cacheCapacityAction;                /**< Maximum resident tile count. */
    MessageCardAction           _tileCountObservationAction;         /**< Estimated tile-index size and high tile-count notice. */
    MessageCardAction           _tileCacheObservationAction;         /**< Estimated tile payload size compared with the cache byte limit. */
    MessageCardAction           _stagingSpaceObservationAction;      /**< Disk-space estimate for import staging, when enabled. */
    ToggleAction                _packedTilesAction;                  /**< Whether serialized tile blocks are grouped into super tiles. */
    IntegralAction              _superTileRowsAction;                /**< Base tile rows per serialized super tile. */
    IntegralAction              _superTileColumnsAction;             /**< Base tile columns per serialized super tile. */
    HorizontalGroupAction       _importPerformanceAction;            /**< Collapsible import or conversion performance settings. */
    StringAction                _importPerformanceSummaryAction;     /**< Read-only summary of active performance settings. */
    VerticalGroupAction         _importPerformanceSettingsAction;    /**< Group containing worker and chunk settings. */
    IntegralAction              _bucketingThreadCountAction;         /**< Maximum workers used to bucket HDF5 input. */
    IntegralAction              _stagingThreadCountAction;           /**< Maximum workers used for staging or tile conversion. */
    IntegralAction              _rowChunkTileFactorAction;           /**< Requested CSR tile-row factor per conversion chunk. */
    OptionAction                _projectResourceStagingPolicyAction; /**< Project-open staging policy stored with imported resources. */
    mv::util::ProjectResourceStagingPolicy _initialProjectResourceStagingPolicy = mv::util::ProjectResourceStagingPolicy::Inherit; /**< Initial policy used for change detection. */
    QString                     _stagingDirectory;                          /**< App-managed directory checked for import staging space. */
    bool                        _showImportPerformanceOptions = false;      /**< Whether the action exposes import worker settings. */
    bool                        _showConversionPerformanceOptions = false;  /**< Whether the performance controls are configured for conversion. */
    bool                        _showImportStagingSpaceObservation = false; /**< Whether to display the import disk-space estimate. */
    bool                        _updatingTileSizePercentage = false;        /**< Guards reciprocal updates between size and percentage controls. */
    bool                        _updatingGeometryFromPolicy = false;        /**< Guards control updates while policy-derived geometry is applied. */
};

}
