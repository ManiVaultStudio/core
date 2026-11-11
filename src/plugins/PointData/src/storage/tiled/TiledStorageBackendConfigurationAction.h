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
    TiledStorageBackendConfigurationAction(QObject* parent, std::uint64_t rows, std::uint64_t columns, std::size_t elementSize, const pointdata::storage::StorageBackendConfiguration& configuration = {}, bool initializeFromConfiguration = false, std::optional<std::uint32_t> currentElementTypeIndex = {}, bool showImportPerformanceOptions = false, std::size_t bucketingThreadCount = 4, std::size_t stagingThreadCount = 4, std::size_t rowChunkTileFactor = 3, bool showImportStagingSpaceObservation = false, const QString& stagingDirectory = QString(), mv::util::ProjectResourceStagingPolicy projectOpenStagingPolicy = mv::util::ProjectResourceStagingPolicy::Inherit);

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

    /** Returns the dataset-level policy used when opening saved projects. */
    [[nodiscard]] mv::util::ProjectResourceStagingPolicy getProjectOpenStagingPolicy() const;

private:
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

    std::uint64_t _rows               = 0;
    std::uint64_t _columns            = 0;
    std::size_t   _elementSize        = 0;
    std::size_t   _cacheCapacityBytes = pointdata::storage::defaultTileCacheCapacityBytes;
    pointdata::storage::StorageBackendConfiguration _initialConfiguration; /**< Original tiled settings used to detect changes in configure mode. */
    bool _hasInitialConfiguration = false; /**< Whether an original tiled configuration was provided. */

    TiledStorageAsymmetryAction _tileAsymmetryAction;
    ToggleAction                _deriveTileDimensionsAction;
    IntegralAction              _tileSizeAction;
    HorizontalGroupAction       _rowTileSizeGroupAction;
    IntegralAction              _rowTileSizeAction;
    DecimalAction              _rowTileSizePercentageAction;
    HorizontalGroupAction       _columnTileSizeGroupAction;
    IntegralAction              _columnTileSizeAction;
    DecimalAction              _columnTileSizePercentageAction;
    IntegralAction              _cacheCapacityAction;
    MessageCardAction              _tileCountObservationAction;
    MessageCardAction              _tileCacheObservationAction;
    MessageCardAction              _stagingSpaceObservationAction;
    ToggleAction                  _packedTilesAction;
    IntegralAction                _superTileRowsAction;
    IntegralAction                _superTileColumnsAction;
    HorizontalGroupAction         _importPerformanceAction;
    StringAction                  _importPerformanceSummaryAction;
    VerticalGroupAction           _importPerformanceSettingsAction;
    IntegralAction                _bucketingThreadCountAction;
    IntegralAction                _stagingThreadCountAction;
    IntegralAction                _rowChunkTileFactorAction;
    OptionAction                  _projectOpenStagingPolicyAction;
    QString                       _stagingDirectory;
    bool                          _showImportPerformanceOptions = false;
    bool                          _showImportStagingSpaceObservation = false;
    bool                          _updatingTileSizePercentage = false;
    bool                          _updatingGeometryFromPolicy = false;
};

}

