// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "StorageBackendConversionAction.h"
#include "../tiled/TiledStorageAsymmetryAction.h"
#include "../tiled/TiledStorageGeometry.h"

#include <actions/IntegralAction.h>
#include <actions/TriggerAction.h>
#include <actions/ToggleAction.h>

namespace mv::gui
{

/**
 * @brief Provides the controls for converting point data to tiled storage.
 *
 * The tiled backend owns these settings because tile geometry and cache size
 * describe its physical representation. Keeping them in this action makes the
 * generic conversion workflow reusable for other backend-specific controls.
 *
 * @author T. Kroes
 */
class POINTDATA_EXPORT TiledStorageBackendConversionAction final : public StorageBackendConversionAction
{
public:
    /**
     * Constructs controls for a matrix with the supplied shape and element size.
     * The tile-size and asymmetry controls calculate a suggested geometry;
     * the row and column dimensions remain directly editable. When requested,
     * the current physical geometry and cache capacity are copied from
     * \p configuration for backend inspection or reconfiguration.
     */
    TiledStorageBackendConversionAction(QObject* parent, std::uint64_t rows, std::uint64_t columns, std::size_t elementSize, const pointdata::storage::StorageBackendConfiguration& configuration = {}, bool initializeFromConfiguration = false);

    /** Returns the tiled backend type represented by this action. */
    pointdata::storage::StorageBackendType getTargetBackendType() const override;

    /** Returns the currently selected physical tile geometry and cache capacity. */
    pointdata::storage::StorageBackendConversionOptionsPtr getConversionOptions() const override;

private:
    void updateTileGeometryFromPolicy();
    void updateCacheCapacity();
    void updatePackedTileControls();

    std::uint64_t _rows        = 0;
    std::uint64_t _columns     = 0;
    std::size_t   _elementSize = 0;

    TiledStorageAsymmetryAction _tileAsymmetryAction;
    IntegralAction              _tileSizeAction;
    IntegralAction              _rowTileSizeAction;
    IntegralAction              _columnTileSizeAction;
    IntegralAction              _cacheCapacityAction;
    ToggleAction                _packedTilesAction;
    IntegralAction              _superTileRowsAction;
    IntegralAction              _superTileColumnsAction;
    TriggerAction               _wizardAction;
};

}
