// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "storage/PointsStorageBackendTypes.h"

#include <QDialog>

class QSpinBox;

/**
 * @brief Collects the configuration used when converting dense point data to tiled storage.
 *
 * The dialog keeps the tile geometry and bounded-cache capacity together so a
 * user can choose a storage layout appropriate for the intended access pattern.
 * It does not perform conversion itself; the owning action validates the
 * accepted configuration and starts the conversion through the Points facade.
 *
 * @author T. Kroes
 */
class StorageBackendConfigurationDialog final : public QDialog
{
public:
    explicit StorageBackendConfigurationDialog(QWidget* parent = nullptr);

    mv::pointdata::storage::StorageBackendConfiguration getConfiguration() const;

private:
    QSpinBox* _rowTileSize    = nullptr;
    QSpinBox* _columnTileSize = nullptr;
    QSpinBox* _cacheCapacity  = nullptr;
};
