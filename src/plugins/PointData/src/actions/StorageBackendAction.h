// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "pointdata_export.h"
#include "../storage/interface/PointsStorageBackendTypes.h"
#include "StorageBackendConfiguratorAction.h"

#include <Dataset.h>

class Points;

/**
 * @brief Displays the storage backend used by a points dataset.
 *
 * StorageBackendAction keeps the backend selection visible alongside the
 * point-data controls. Selecting another supported backend opens controls
 * supplied by that target backend and starts the conversion after acceptance.
 * The adjacent configuration trigger opens the active backend's controls for
 * runtime reconfiguration without changing the backend type.
 *
 * @author T. Kroes
 */
class POINTDATA_EXPORT StorageBackendAction final : public StorageBackendConfiguratorAction
{
    Q_OBJECT

public:
    /** Constructs a storage backend action for \p points. */
    StorageBackendAction(QObject* parent, const mv::Dataset<Points>& points);

private:
    void updateBackendType();
    void changeBackendType(const QString& backendType);
    void convertBackend(mv::pointdata::storage::StorageBackendType type, mv::pointdata::storage::StorageBackendConversionOptionsPtr options = {});

    mv::Dataset<Points>         _points;                 /**< Points dataset whose backend is displayed and converted. */
    bool                        _updatingBackendType = false; /**< Whether the selector is being synchronized with the dataset. */
    bool                        _conversionInProgress = false; /**< Whether a backend conversion is currently running. */
};
