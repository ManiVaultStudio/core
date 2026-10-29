// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "TiledStorageBackendConfigurationAction.h"
#include "StorageBackendObservationAction.h"

namespace mv::gui
{

/**
 * @brief Provides tiled-storage controls for point-data backend conversion.
 *
 * This specialization keeps conversion call sites explicit while reusing the
 * tiled configuration controls used by import and other setup workflows.
 *
 * @author T. Kroes
 */
class POINTDATA_EXPORT TiledStorageBackendConversionAction final : public TiledStorageBackendConfigurationAction
{
public:
    /** Constructs tiled-storage controls for converting a matrix with the supplied shape and element size. */
    TiledStorageBackendConversionAction(QObject* parent, std::uint64_t rows, std::uint64_t columns, std::size_t elementSize, const pointdata::storage::StorageBackendConfiguration& configuration = {}, bool initializeFromConfiguration = false, bool materializesFullMatrix = false);

    /** Returns false when materializing all destination tiles exceeds available physical memory. */
    bool canApply() const override;

private:
    bool _canApply = true;
    StorageBackendObservationAction _memoryObservationAction;
};

}
