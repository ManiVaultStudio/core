// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "../interface/StorageBackendConversionAction.h"

#include <actions/MessageCardAction.h>

namespace mv::gui
{

/**
 * @brief Displays a dense-allocation preflight when converting a backend to dense storage.
 *
 * Estimates the destination matrix's dense byte size and compares it with
 * currently available physical memory where the operating system exposes it.
 * The reading can change before the conversion starts.
 *
 * @author Thomas Kroes
 */
class POINTDATA_EXPORT DenseStorageBackendConversionAction final : public StorageBackendConversionAction
{
public:
    /** Constructs an in-memory conversion preflight for a matrix with the supplied shape and current element type. */
    DenseStorageBackendConversionAction(QObject* parent, std::uint64_t rows, std::uint64_t columns, std::uint32_t currentElementTypeIndex);

    /** Returns the dense backend type. */
    pointdata::storage::StorageBackendType getTargetBackendType() const override;
    /** Returns no backend-specific options for dense storage. */
    pointdata::storage::StorageBackendConfigurationOptionsPtr getConversionOptions() const override;
    /** Returns false when the dense allocation exceeds addressable or currently available physical memory. */
    bool canApply() const override;

private:
    void updateMemoryObservation();

    std::uint64_t _rows = 0;                    /**< Matrix row count used by the allocation estimate. */
    std::uint64_t _columns = 0;                 /**< Matrix column count used by the allocation estimate. */
    bool _canApply = true;                      /**< Whether the latest allocation preflight allows conversion. */
    MessageCardAction _memoryObservationAction; /**< Displays the estimated dense allocation and memory warning. */
};

}
