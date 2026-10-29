// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "StorageBackendConversionAction.h"
#include "StorageBackendObservationAction.h"

#include <actions/OptionAction.h>

namespace mv::gui
{

/**
 * @brief Selects CSR or CSC layout and reports the memory implications of sparse conversion.
 *
 * Sparse conversion scans the complete matrix and stores exact nonzero values.
 * The reported sparse footprint is an upper bound assuming every matrix value
 * is nonzero; actual memory use depends on the data.
 */
class POINTDATA_EXPORT SparseStorageBackendConversionAction final : public StorageBackendConversionAction
{
public:
    /** Constructs sparse conversion controls for a matrix with the supplied shape and element size. */
    SparseStorageBackendConversionAction(QObject* parent, std::uint64_t rows, std::uint64_t columns, std::size_t elementSize);

    /** Returns the sparse backend type. */
    pointdata::storage::StorageBackendType getTargetBackendType() const override;
    /** Returns the selected compressed orientation. */
    pointdata::storage::StorageBackendConfigurationOptionsPtr getConversionOptions() const override;
    /** Returns false when matrix dimensions or pointer arrays cannot be represented. */
    bool canApply() const override;

private:
    void updateObservation();

    std::uint64_t _rows = 0;
    std::uint64_t _columns = 0;
    std::size_t _elementSize = 0;
    OptionAction _layoutAction;
    StorageBackendObservationAction _memoryObservationAction;
};

}
