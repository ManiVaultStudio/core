// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "StorageBackendConversionAction.h"

#include <actions/IntegralAction.h>

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
    explicit TiledStorageBackendConversionAction(QObject* parent = nullptr);

    pointdata::storage::StorageBackendType getTargetBackendType() const override;
    pointdata::storage::StorageBackendConversionOptionsPtr getConversionOptions() const override;

private:
    IntegralAction _rowTileSizeAction;
    IntegralAction _columnTileSizeAction;
    IntegralAction _cacheCapacityAction;
};

}
