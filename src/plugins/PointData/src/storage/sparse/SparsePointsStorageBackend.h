// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2026 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "../interface/PointsStorageBackend.h"

#include <memory>
#include <vector>

namespace mv::pointdata::storage
{

/** Compressed sparse layout used by SparsePointsStorageBackend. */
enum class SparseMatrixLayout
{
    RowCompressed,    /**< CSR: fast row access and row-wise scans. */
    ColumnCompressed  /**< CSC: fast column access and column-wise scans. */
};

/**
 * @brief Immutable float-valued sparse matrix backend in CSR or CSC form.
 *
 * The backend owns the compressed pointer, index, and value arrays. Indices
 * within each compressed vector must be strictly increasing, so duplicate
 * coordinates are rejected and explicit zero values are retained as supplied.
 * Row-based access is efficient for CSR; CSC keeps the analogous advantage for
 * columns. Requests through the row-major Points interface are supported in
 * either layout, with CSC row reads requiring a search through each column.
 * Dense rows and complete materialization allocate only the requested output.
 *
 * The backend is read-only. A dense materialization is explicit and can require
 * substantial memory for highly sparse, large matrices.
 *
 * @author Thomas Kroes
 */
class POINTDATA_EXPORT SparsePointsStorageBackend final : public PointsStorageBackend
{
public:
    /** Creates a validated sparse backend. Pointer arrays include the terminal nonzero count. */
    SparsePointsStorageBackend(std::uint64_t rows, std::uint64_t columns, SparseMatrixLayout layout, std::vector<std::uint64_t> pointers, std::vector<std::uint64_t> indices, std::vector<float> values);

    StorageBackendType getType() const override;
    StorageBackendCapabilities getCapabilities() const override;
    std::unique_ptr<mv::gui::StorageBackendConversionAction> createConversionAction(StorageBackendType targetType, QObject* parent) const override;
    void applyConfiguration(const StorageBackendConversionOptionsPtr& options) override;
    std::uint64_t getNumPoints() const override;
    std::uint64_t getNumDimensions() const override;
    std::size_t getElementSize() const override;
    std::uint32_t getElementTypeIndex() const override;
    PointDataAccessView getRow(std::uint64_t row) const override;
    void visitRange(std::uint64_t firstRow, std::uint64_t rowCount, std::uint64_t firstColumn, std::uint64_t columnCount, const PointDataAccessVisitor& visitor) const override;
    float getValueAt(std::uint64_t flatIndex) const override;
    void materialize(char* destination) const override;

    /** Returns the compressed layout. */
    SparseMatrixLayout getLayout() const;

    /** Returns the number of stored values, including explicitly stored zeros. */
    std::uint64_t getNumNonZeros() const;

private:
    float valueAt(std::uint64_t row, std::uint64_t column) const;
    void copyRowRange(std::uint64_t row, std::uint64_t firstColumn, std::uint64_t columnCount, float* destination) const;

    std::uint64_t              _rows     = 0;
    std::uint64_t              _columns  = 0;
    SparseMatrixLayout         _layout   = SparseMatrixLayout::RowCompressed;
    std::vector<std::uint64_t> _pointers;
    std::vector<std::uint64_t> _indices;
    std::vector<float>         _values;
};

}
