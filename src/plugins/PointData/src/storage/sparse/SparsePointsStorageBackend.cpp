// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2026 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "SparsePointsStorageBackend.h"

#include <algorithm>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <utility>

namespace mv::pointdata::storage
{

SparsePointsStorageBackend::SparsePointsStorageBackend(const std::uint64_t rows, const std::uint64_t columns, const SparseMatrixLayout layout, std::vector<std::uint64_t> pointers, std::vector<std::uint64_t> indices, std::vector<float> values) :
    _rows     (rows),
    _columns  (columns),
    _layout   (layout),
    _pointers (std::move(pointers)),
    _indices  (std::move(indices)),
    _values   (std::move(values))
{
    if (_layout != SparseMatrixLayout::RowCompressed && _layout != SparseMatrixLayout::ColumnCompressed)
        throw std::invalid_argument("Sparse matrix layout is invalid");

    const auto compressedSize   = _layout == SparseMatrixLayout::RowCompressed ? _rows : _columns;
    const auto uncompressedSize = _layout == SparseMatrixLayout::RowCompressed ? _columns : _rows;
    const auto invalidPointers  = compressedSize >= std::numeric_limits<std::size_t>::max() || _pointers.size() != compressedSize + 1 || _indices.size() != _values.size() || _pointers.empty() || _pointers.front() != 0 || _pointers.back() != _values.size();

    if (invalidPointers)
        throw std::invalid_argument("Sparse matrix pointer and value arrays are inconsistent");

    for (std::size_t compressedIndex = 0; compressedIndex < compressedSize; ++compressedIndex)
    {
        const auto begin = _pointers[compressedIndex];
        const auto end   = _pointers[compressedIndex + 1];

        if (begin > end || end > _indices.size())
            throw std::invalid_argument("Sparse matrix pointers must be monotonic and within the index array");

        std::uint64_t previousIndex = 0;
        bool hasPreviousIndex = false;

        for (auto index = begin; index < end; ++index)
        {
            const auto uncompressedIndex = _indices[static_cast<std::size_t>(index)];

            if (uncompressedIndex >= uncompressedSize || (hasPreviousIndex && uncompressedIndex <= previousIndex))
                throw std::invalid_argument("Sparse matrix indices must be in range, sorted, and unique within each compressed segment");

            previousIndex = uncompressedIndex;
            hasPreviousIndex = true;
        }
    }
}

StorageBackendType SparsePointsStorageBackend::getType() const
{
    return StorageBackendType::Sparse;
}

StorageBackendCapabilities SparsePointsStorageBackend::getCapabilities() const
{
    return StorageBackendCapability::RandomAccess | StorageBackendCapability::RangeAccess | StorageBackendCapability::Materializable;
}

std::unique_ptr<mv::gui::StorageBackendConversionAction> SparsePointsStorageBackend::createConversionAction(const StorageBackendType, QObject*) const
{
    return {};
}

void SparsePointsStorageBackend::applyConfiguration(const StorageBackendConversionOptionsPtr& options)
{
    if (options)
        throw std::invalid_argument("Sparse point-data storage does not accept runtime configuration");
}

std::uint64_t SparsePointsStorageBackend::getNumPoints() const
{
    return _rows;
}

std::uint64_t SparsePointsStorageBackend::getNumDimensions() const
{
    return _columns;
}

std::size_t SparsePointsStorageBackend::getElementSize() const
{
    return sizeof(float);
}

std::uint32_t SparsePointsStorageBackend::getElementTypeIndex() const
{
    return 0;
}

PointDataAccessView SparsePointsStorageBackend::getRow(const std::uint64_t row) const
{
    if (row >= _rows)
        return {};

    auto values = std::make_shared<std::vector<float>>(static_cast<std::size_t>(_columns), 0.0f);
    copyRowRange(row, 0, _columns, values->data());

    PointDataAccessView view;
    view.data               = values->data();
    view.rows               = 1;
    view.columns            = _columns;
    view.rowStrideBytes     = _columns * sizeof(float);
    view.elementTypeIndex   = getElementTypeIndex();
    view.elementSizeBytes   = static_cast<std::uint32_t>(sizeof(float));
    view.lifetime           = std::shared_ptr<const void>(values, values->data());
    return view;
}

void SparsePointsStorageBackend::visitRange(const std::uint64_t firstRow, const std::uint64_t rowCount, const std::uint64_t firstColumn, const std::uint64_t columnCount, const PointDataAccessVisitor& visitor) const
{
    if (!visitor || firstRow > _rows || rowCount > _rows - firstRow || firstColumn > _columns || columnCount > _columns - firstColumn)
        throw std::out_of_range("Sparse point-data range is invalid");

    if (rowCount == 0 || columnCount == 0)
        return;

    for (auto row = firstRow; row < firstRow + rowCount; ++row)
    {
        auto values = std::make_shared<std::vector<float>>(static_cast<std::size_t>(columnCount), 0.0f);
        copyRowRange(row, firstColumn, columnCount, values->data());

        PointDataAccessView view;
        view.data               = values->data();
        view.rows               = 1;
        view.columns            = columnCount;
        view.rowStrideBytes     = columnCount * sizeof(float);
        view.elementTypeIndex   = getElementTypeIndex();
        view.elementSizeBytes   = static_cast<std::uint32_t>(sizeof(float));
        view.lifetime           = std::shared_ptr<const void>(values, values->data());
        visitor(view);
    }
}

float SparsePointsStorageBackend::getValueAt(const std::uint64_t flatIndex) const
{
    if (_columns == 0 || (_rows != 0 && _columns > std::numeric_limits<std::uint64_t>::max() / _rows) || flatIndex >= _rows * _columns)
        throw std::out_of_range("Sparse point-data index is invalid");

    return valueAt(flatIndex / _columns, flatIndex % _columns);
}

void SparsePointsStorageBackend::materialize(char* destination) const
{
    if (!destination)
        return;

    if (_rows != 0 && _columns > std::numeric_limits<std::uint64_t>::max() / _rows)
        throw std::overflow_error("Sparse matrix dimensions overflow the dense element count");

    const auto elementCount = _rows * _columns;
    if (elementCount > std::numeric_limits<std::size_t>::max() / sizeof(float))
        throw std::overflow_error("Sparse matrix is too large to materialize");

    std::memset(destination, 0, static_cast<std::size_t>(elementCount) * sizeof(float));

    if (_layout == SparseMatrixLayout::RowCompressed)
    {
        for (std::uint64_t row = 0; row < _rows; ++row)
        {
            for (auto index = _pointers[static_cast<std::size_t>(row)]; index < _pointers[static_cast<std::size_t>(row + 1)]; ++index)
            {
                const auto column = _indices[static_cast<std::size_t>(index)];
                const auto flatIndex = row * _columns + column;
                std::memcpy(destination + flatIndex * sizeof(float), &_values[static_cast<std::size_t>(index)], sizeof(float));
            }
        }
    }
    else
    {
        for (std::uint64_t column = 0; column < _columns; ++column)
        {
            for (auto index = _pointers[static_cast<std::size_t>(column)]; index < _pointers[static_cast<std::size_t>(column + 1)]; ++index)
            {
                const auto row = _indices[static_cast<std::size_t>(index)];
                const auto flatIndex = row * _columns + column;
                std::memcpy(destination + flatIndex * sizeof(float), &_values[static_cast<std::size_t>(index)], sizeof(float));
            }
        }
    }
}

SparseMatrixLayout SparsePointsStorageBackend::getLayout() const
{
    return _layout;
}

std::uint64_t SparsePointsStorageBackend::getNumNonZeros() const
{
    return _values.size();
}

float SparsePointsStorageBackend::valueAt(const std::uint64_t row, const std::uint64_t column) const
{
    const auto compressedIndex = _layout == SparseMatrixLayout::RowCompressed ? row : column;
    const auto targetIndex     = _layout == SparseMatrixLayout::RowCompressed ? column : row;
    const auto begin           = _pointers[static_cast<std::size_t>(compressedIndex)];
    const auto end             = _pointers[static_cast<std::size_t>(compressedIndex + 1)];
    const auto first           = _indices.begin() + static_cast<std::ptrdiff_t>(begin);
    const auto last            = _indices.begin() + static_cast<std::ptrdiff_t>(end);
    const auto found           = std::lower_bound(first, last, targetIndex);

    if (found == last || *found != targetIndex)
        return 0.0f;

    return _values[static_cast<std::size_t>(found - _indices.begin())];
}

void SparsePointsStorageBackend::copyRowRange(const std::uint64_t row, const std::uint64_t firstColumn, const std::uint64_t columnCount, float* destination) const
{
    if (_layout == SparseMatrixLayout::RowCompressed)
    {
        const auto begin = _pointers[static_cast<std::size_t>(row)];
        const auto end = _pointers[static_cast<std::size_t>(row + 1)];
        const auto first = _indices.begin() + static_cast<std::ptrdiff_t>(begin);
        const auto last = _indices.begin() + static_cast<std::ptrdiff_t>(end);
        const auto rangeBegin = std::lower_bound(first, last, firstColumn);
        const auto rangeEnd = std::lower_bound(rangeBegin, last, firstColumn + columnCount);

        for (auto index = rangeBegin; index != rangeEnd; ++index)
        {
            const auto valueIndex = static_cast<std::size_t>(index - _indices.begin());
            destination[*index - firstColumn] = _values[valueIndex];
        }

        return;
    }

    for (auto column = firstColumn; column < firstColumn + columnCount; ++column)
    {
        const auto value = valueAt(row, column);
        destination[column - firstColumn] = value;
    }
}

}
