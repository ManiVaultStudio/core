// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2026 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "../interface/PointsStorageBackend.h"

#include <memory>
#include <vector>

namespace mv::pointdata::storage
{

/**
 * @brief Compressed orientation of sparse matrix storage.
 *
 * The pointer array has one entry per compressed row or column plus a final
 * entry equal to the number of stored values. Indices are strictly increasing
 * within each compressed segment.
 */
enum class SparseMatrixLayout
{
    RowCompressed,    /**< CSR: each segment stores increasing column indices for one row. */
    ColumnCompressed  /**< CSC: each segment stores increasing row indices for one column. */
};

/**
 * @brief Immutable typed sparse matrix backend in CSR or CSC form.
 *
 * This backend lets a sparse matrix satisfy the common point-data access
 * contract without keeping a dense matrix as its backing representation.
 *
 * The backend owns the compressed pointer, index, and value arrays. Indices
 * within each compressed segment must be strictly increasing, so duplicate
 * coordinates are rejected and explicit zero values are retained as supplied.
 * CSR favors row reads. CSC stores each column contiguously, which suits
 * column-oriented sources or consumers; the Points backend interface remains
 * row-major, so CSC row and range reads search the requested columns. The
 * generic scalar accessor converts stored values to float; row and range views
 * preserve the configured PointData element type.
 *
 * The backend is read-only and retains the supplied PointData element type.
 * Row and range requests allocate dense output for the requested cells. Full
 * materialization writes a dense row-major matrix into caller-owned memory and
 * may require substantial memory for large, sparse matrices. The backend
 * owns its input arrays for its lifetime; returned row and range views retain
 * their temporary buffers through their lifetime tokens.
 *
 * @author Thomas Kroes
 */
class POINTDATA_EXPORT SparsePointsStorageBackend final : public PointsStorageBackend
{
public:
    /**
     * Creates a validated sparse backend and stores the supplied arrays in
     * backend-owned storage. Passing rvalue vectors moves their contents;
     * passing lvalues copies them.
     *
     * @param rows Number of matrix rows.
     * @param columns Number of matrix columns.
     * @param layout CSR or CSC compressed orientation.
     * @param pointers Compressed segment offsets, with one entry per row (CSR) or column (CSC), plus a terminal stored-value count. Offsets start at zero and are monotonic.
     * @param indices Sorted, unique column indices (CSR) or row indices (CSC) for each compressed segment.
     * @param elementTypeIndex PointData element-type index used to interpret values; supported indices are 0 through 7.
     * @param elementSize Size in bytes of each stored value; it must match the selected element type.
     * @param values Byte-packed values corresponding to indices. Explicit zeros are retained.
     * @throws std::invalid_argument If the element type or layout is unsupported, dimensions cannot be represented by the pointer array, or array sizes, pointer sequence, or indices are invalid.
     */
    SparsePointsStorageBackend(std::uint64_t rows, std::uint64_t columns, SparseMatrixLayout layout, std::uint32_t elementTypeIndex, std::size_t elementSize, std::vector<std::uint64_t> pointers, std::vector<std::uint64_t> indices, std::vector<std::uint8_t> values);

    /** @brief Returns the sparse backend type. */
    StorageBackendType getType() const override;

    /** @brief Returns random access, range access, and materialization capabilities; the backend is read-only. */
    StorageBackendCapabilities getCapabilities() const override;

    /** @brief Returns no conversion action because this sparse backend does not provide conversion controls. */
    std::unique_ptr<mv::gui::StorageBackendConversionAction> createConversionAction(StorageBackendType targetType, QObject* parent) const override;

    /** @brief Accepts no options; throws std::invalid_argument when options is non-null. */
    void applyConfiguration(const StorageBackendConfigurationOptionsPtr& options) override;

    /** @brief Returns the matrix row count. */
    std::uint64_t getNumPoints() const override;

    /** @brief Returns the matrix column count. */
    std::uint64_t getNumDimensions() const override;

    /** @brief Returns the stored element size. */
    std::size_t getElementSize() const override;

    /** @brief Returns the stored PointData element-type index. */
    std::uint32_t getElementTypeIndex() const override;

    /**
     * Returns a dense row view backed by an owned temporary buffer. Retain the
     * view's lifetime token for as long as its data pointer is used.
     *
     * @param row Zero-based matrix row.
     * @return Empty view when row is out of range; otherwise a dense view in the stored element type.
     * @throws std::overflow_error If the dense row byte count exceeds the process address space.
     */
    PointDataAccessView getRow(std::uint64_t row) const override;

    /**
     * Visits each requested row as a dense row-major view.
     *
     * An empty range performs no callbacks. Invalid bounds or an empty visitor
     * throw std::out_of_range. Each view owns its temporary output through its
     * lifetime token and remains valid while that token is retained.
     *
     * @param firstRow First zero-based matrix row.
     * @param rowCount Number of rows to visit.
     * @param firstColumn First zero-based matrix column.
     * @param columnCount Number of columns to include in each visited row.
     * @param visitor Callback invoked once per row with a dense view of the requested columns.
     * @throws std::out_of_range If the range exceeds the matrix bounds or visitor is empty.
     * @throws std::overflow_error If a dense output row exceeds the process address space.
     */
    void visitRange(std::uint64_t firstRow, std::uint64_t rowCount, std::uint64_t firstColumn, std::uint64_t columnCount, const PointDataAccessVisitor& visitor) const override;

    /**
     * Reads one row-major element and converts the stored value to float.
     *
     * @param flatIndex Zero-based index in the dense row-major matrix shape.
     * @return Stored value, or zero when the coordinate is not stored.
     * @throws std::out_of_range If the index is outside the matrix or the dense dimensions overflow.
     */
    float getValueAt(std::uint64_t flatIndex) const override;

    /**
     * Writes the complete dense row-major matrix in the stored element type to caller-owned memory.
     *
     * The destination must hold getNumPoints() * getNumDimensions() elements of getElementSize() bytes.
     * A null destination is a no-op. Throws std::overflow_error if the dense
     * output size cannot be represented by the process address space.
     * @param destination Caller-owned buffer receiving row-major values.
     * @throws std::overflow_error If the dense element or byte count overflows.
     */
    void materialize(char* destination) const override;

    /** @brief Returns whether the owned arrays are CSR or CSC. */
    SparseMatrixLayout getLayout() const;

    /** @brief Returns the number of stored values, including explicitly stored zeros. */
    std::uint64_t getNumNonZeros() const;

    /** Returns the logical byte size of the compressed arrays, excluding allocator overhead. */
    std::uint64_t getStorageSizeBytes() const;

    /** Returns the backend-owned segment pointers. */
    const std::vector<std::uint64_t>& getPointers() const;

    /** Returns the backend-owned sorted sparse indices. */
    const std::vector<std::uint64_t>& getIndices() const;

    /** Returns the backend-owned byte-packed values. */
    const std::vector<std::uint8_t>& getValues() const;

private:
    float valueAt(std::uint64_t row, std::uint64_t column) const;
    void copyRowRange(std::uint64_t row, std::uint64_t firstColumn, std::uint64_t columnCount, std::uint8_t* destination) const;

    std::uint64_t              _rows             = 0;
    std::uint64_t              _columns          = 0;
    SparseMatrixLayout         _layout           = SparseMatrixLayout::RowCompressed;
    std::uint32_t              _elementTypeIndex = 0;
    std::size_t                _elementSize      = sizeof(float);
    std::vector<std::uint64_t> _pointers;
    std::vector<std::uint64_t> _indices;
    std::vector<std::uint8_t>  _values;
};

}
