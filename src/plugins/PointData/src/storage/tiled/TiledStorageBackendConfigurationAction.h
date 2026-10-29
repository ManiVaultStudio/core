// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "../interface/StorageBackendConversionAction.h"
#include "TiledStorageAsymmetryAction.h"
#include "TiledStorageGeometry.h"

#include <actions/MessageCardAction.h>
#include <actions/IntegralAction.h>
#include <actions/ToggleAction.h>

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
    TiledStorageBackendConfigurationAction(QObject* parent, std::uint64_t rows, std::uint64_t columns, std::size_t elementSize, const pointdata::storage::StorageBackendConfiguration& configuration = {}, bool initializeFromConfiguration = false, std::optional<std::uint32_t> currentElementTypeIndex = {});

    /** Returns the tiled backend type represented by this action. */
    pointdata::storage::StorageBackendType getTargetBackendType() const override;

    /** Returns the currently selected physical tile geometry and cache capacity. */
    pointdata::storage::StorageBackendConfigurationOptionsPtr getConversionOptions() const override;

    /** Returns whether the selected element type or tiled settings differ from their initial values. */
    bool hasChanges() const override;

    /** Returns the current tiled-storage configuration. */
    pointdata::storage::StorageBackendConfiguration getConfiguration() const;

private:
    void updateTileGeometryFromPolicy();
    void updateTargetTileSizeFromGeometry();
    void updateCacheCapacity();
    void updateStorageObservations();
    void updatePackedTileControls();

    std::uint64_t _rows               = 0;
    std::uint64_t _columns            = 0;
    std::size_t   _elementSize        = 0;
    std::size_t   _cacheCapacityBytes = pointdata::storage::defaultTileCacheCapacityBytes;
    pointdata::storage::StorageBackendConfiguration _initialConfiguration; /**< Original tiled settings used to detect changes in configure mode. */
    bool _hasInitialConfiguration = false; /**< Whether an original tiled configuration was provided. */

    TiledStorageAsymmetryAction _tileAsymmetryAction;
    IntegralAction              _tileSizeAction;
    IntegralAction              _rowTileSizeAction;
    IntegralAction              _columnTileSizeAction;
    IntegralAction              _cacheCapacityAction;
    MessageCardAction              _tileCountObservationAction;
    MessageCardAction              _tileCacheObservationAction;
    ToggleAction                  _packedTilesAction;
    IntegralAction                _superTileRowsAction;
    IntegralAction                _superTileColumnsAction;
};

}

