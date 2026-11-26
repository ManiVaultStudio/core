// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "../interface/StorageBackendConversionAction.h"

#include <actions/HorizontalGroupAction.h>
#include <actions/IntegralAction.h>
#include <actions/OptionAction.h>
#include <actions/StringAction.h>
#include <actions/VerticalGroupAction.h>

namespace mv::gui
{

/**
 * @brief Selects CSR or CSC layout for sparse conversion.
 *
 * Sparse conversion scans the complete matrix and stores exact nonzero values.
 * The element type can be changed as part of conversion, with values outside
 * an integer type's range causing conversion to fail without replacing the
 * current representation.
 *
 * @author Thomas Kroes
 */
class POINTDATA_EXPORT SparseStorageBackendConversionAction final : public StorageBackendConversionAction
{
public:
    /** Constructs sparse conversion controls for a matrix with the supplied shape and current element type. */
    SparseStorageBackendConversionAction(QObject* parent, std::uint64_t rows, std::uint64_t columns, std::uint32_t currentElementTypeIndex, bool columnCompressed = false, std::size_t workerCount = 4, std::size_t chunkRows = 128);

    /** Returns the sparse backend type. */
    pointdata::storage::StorageBackendType getTargetBackendType() const override;
    /** Returns the selected compressed orientation. */
    pointdata::storage::StorageBackendConfigurationOptionsPtr getConversionOptions() const override;
    /** Returns false when matrix dimensions or pointer arrays cannot be represented. */
    bool canApply() const override;
    /** Returns whether the element type or compressed orientation differs from its initial value. */
    bool hasChanges() const override;

private:
    std::uint64_t _rows = 0;                        /**< Matrix row count used for the size and representability estimates. */
    std::uint64_t _columns = 0;                     /**< Matrix column count used for the size and representability estimates. */
    bool _initialColumnCompressed = false;          /**< Original compressed orientation for configuration change detection. */
    OptionAction _layoutAction;                     /**< CSR or CSC selection. */
    HorizontalGroupAction _performanceAction;       /**< Collapsible conversion performance controls. */
    StringAction _performanceSummaryAction;         /**< Read-only summary of conversion settings. */
    VerticalGroupAction _performanceSettingsAction; /**< Group containing worker and chunk settings. */
    IntegralAction _workerCountAction;              /**< Maximum conversion worker count. */
    IntegralAction _chunkRowsAction;                /**< Source rows assigned to a sparse conversion chunk. */
    std::size_t _initialWorkerCount = 4;            /**< Worker count used to detect changes from initial settings. */
    std::size_t _initialChunkRows = 128;            /**< Chunk height used to detect changes from initial settings. */
};

}
