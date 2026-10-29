// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "StorageBackendConversionAction.h"
#include "StorageBackendObservationAction.h"

namespace mv::gui
{

/**
 * @brief Displays a dense-allocation preflight when converting a backend to eager storage.
 *
 * Estimates the destination matrix's dense byte size and compares it with
 * currently available physical memory where the operating system exposes it.
 * The reading can change before the conversion starts.
 */
class POINTDATA_EXPORT EagerStorageBackendConversionAction final : public StorageBackendConversionAction
{
public:
    /** Constructs an in-memory conversion preflight for a matrix with the supplied shape and element size. */
    EagerStorageBackendConversionAction(QObject* parent, std::uint64_t rows, std::uint64_t columns, std::size_t elementSize);

    /** Returns the eager backend type. */
    pointdata::storage::StorageBackendType getTargetBackendType() const override;
    /** Returns no backend-specific options for eager storage. */
    pointdata::storage::StorageBackendConfigurationOptionsPtr getConversionOptions() const override;
    /** Returns false when the dense allocation exceeds addressable or currently available physical memory. */
    bool canApply() const override;

private:
    std::uint64_t _rows = 0;
    std::uint64_t _columns = 0;
    std::size_t _elementSize = 0;
    bool _canApply = true;
    StorageBackendObservationAction _memoryObservationAction;
};

}
