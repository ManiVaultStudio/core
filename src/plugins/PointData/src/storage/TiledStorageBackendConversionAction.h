// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "StorageBackendConversionAction.h"
#include "TiledStorageDominanceAction.h"
#include "TiledStorageGeometry.h"

#include <actions/StringAction.h>

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
    TiledStorageBackendConversionAction(QObject* parent, std::uint64_t rows, std::uint64_t columns, const pointdata::storage::StorageBackendConfiguration& configuration = {}, bool initializeFromConfiguration = false);

    pointdata::storage::StorageBackendType getTargetBackendType() const override;
    pointdata::storage::StorageBackendConversionOptionsPtr getConversionOptions() const override;

private:
    void updateTileGeometry();

    std::uint64_t _rows    = 0;
    std::uint64_t _columns = 0;

    TiledStorageDominanceAction _tileDominanceAction;
    StringAction                _tileGeometryAction;
    IntegralAction _cacheCapacityAction;
};

}
