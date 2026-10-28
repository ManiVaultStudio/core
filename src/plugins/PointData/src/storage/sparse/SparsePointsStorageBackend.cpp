// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2026 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "SparsePointsStorageBackend.h"

#include "../actions/StorageBackendConversionAction.h"

#include <biovault_bfloat16/biovault_bfloat16.h>

#include <algorithm>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <utility>

namespace mv::pointdata::storage
{

namespace
{
    template<typename T>
    float readValueAsFloat(const std::uint8_t* value)
    {
        T typedValue;
        std::memcpy(&typedValue, value, sizeof(T));
        return static_cast<float>(typedValue);
    }
}

SparsePointsStorageBackend::SparsePointsStorageBackend(const std::uint64_t rows, const std::uint64_t columns, const SparseMatrixLayout layout, const std::uint32_t elementTypeIndex, const std::size_t elementSize, std::vector<std::uint64_t> pointers, std::vector<std::uint64_t> indices, std::vector<std::uint8_t> values) :
    _rows(rows),
    _columns(columns),
    _layout(layout),
    _elementTypeIndex(elementTypeIndex),
    _elementSize(elementSize),
    _pointers(std::move(pointers)),
    _indices(std::move(indices)),
    _values(std::move(values))
{
    if (_layout != SparseMatrixLayout::RowCompressed && _layout != SparseMatrixLayout::ColumnCompressed)
        throw std::invalid_argument("Sparse matrix layout is invalid");

    const auto compressedSize     = _layout == SparseMatrixLayout::RowCompressed ? _rows : _columns;
    const auto uncompressedSize   = _layout == SparseMatrixLayout::RowCompressed ? _columns : _rows;
    const auto invalidElementType = _elementTypeIndex > 7 || (_elementTypeIndex == 0 && _elementSize != 4) ||
        (_elementTypeIndex == 1 && _elementSize != 2) || (_elementTypeIndex >= 2 && _elementTypeIndex <= 3 && _elementSize != 4) ||
        (_elementTypeIndex >= 4 && _elementTypeIndex <= 5 && _elementSize != 2) || (_elementTypeIndex >= 6 && _elementTypeIndex <= 7 && _elementSize != 1);
    const auto invalidValues     = _elementSize == 0 || _values.size() % _elementSize != 0 || _indices.size() != _values.size() / _elementSize;
    const auto invalidPointers   = invalidElementType || invalidValues || compressedSize >= std::numeric_limits<std::size_t>::max() ||
        _pointers.size() != compressedSize + 1 || _pointers.empty() || _pointers.front() != 0 || _pointers.back() != _indices.size();

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
    return _elementSize;
}

std::uint32_t SparsePointsStorageBackend::getElementTypeIndex() const
{
    return _elementTypeIndex;
}

PointDataAccessView SparsePointsStorageBackend::getRow(const std::uint64_t row) const
{
    if (row >= _rows)
        return {};

    if (_columns > std::numeric_limits<std::size_t>::max() / _elementSize)
        throw std::overflow_error("Sparse row is too large");

    auto values = std::make_shared<std::vector<std::uint8_t>>(static_cast<std::size_t>(_columns) * _elementSize, 0);

    copyRowRange(row, 0, _columns, values->data());

    PointDataAccessView view;
    view.data               = values->data();
    view.rows               = 1;
    view.columns            = _columns;
    view.rowStrideBytes     = _columns * _elementSize;
    view.elementTypeIndex   = getElementTypeIndex();
    view.elementSizeBytes   = static_cast<std::uint32_t>(_elementSize);
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
        if (columnCount > std::numeric_limits<std::size_t>::max() / _elementSize)
            throw std::overflow_error("Sparse range row is too large");

        auto values = std::make_shared<std::vector<std::uint8_t>>(static_cast<std::size_t>(columnCount) * _elementSize, 0);
        copyRowRange(row, firstColumn, columnCount, values->data());

        PointDataAccessView view;
        view.data               = values->data();
        view.rows               = 1;
        view.columns            = columnCount;
        view.rowStrideBytes     = columnCount * _elementSize;
        view.elementTypeIndex   = getElementTypeIndex();
        view.elementSizeBytes   = static_cast<std::uint32_t>(_elementSize);
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
    if (_elementSize == 0 || elementCount > std::numeric_limits<std::size_t>::max() / _elementSize)
        throw std::overflow_error("Sparse matrix is too large to materialize");

    std::memset(destination, 0, static_cast<std::size_t>(elementCount) * _elementSize);

    if (_layout == SparseMatrixLayout::RowCompressed)
    {
        for (std::uint64_t row = 0; row < _rows; ++row)
        {
            for (auto index = _pointers[static_cast<std::size_t>(row)]; index < _pointers[static_cast<std::size_t>(row + 1)]; ++index)
            {
                const auto column = _indices[static_cast<std::size_t>(index)];
                const auto flatIndex = row * _columns + column;
                std::memcpy(destination + flatIndex * _elementSize, _values.data() + static_cast<std::size_t>(index) * _elementSize, _elementSize);
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
                std::memcpy(destination + flatIndex * _elementSize, _values.data() + static_cast<std::size_t>(index) * _elementSize, _elementSize);
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
    return _elementSize == 0 ? 0 : _values.size() / _elementSize;
}

std::uint64_t SparsePointsStorageBackend::getStorageSizeBytes() const
{
    return static_cast<std::uint64_t>(_pointers.size()) * sizeof(std::uint64_t) + static_cast<std::uint64_t>(_indices.size()) * sizeof(std::uint64_t) + _values.size();
}

const std::vector<std::uint64_t>& SparsePointsStorageBackend::getPointers() const
{
    return _pointers;
}

const std::vector<std::uint64_t>& SparsePointsStorageBackend::getIndices() const
{
    return _indices;
}

const std::vector<std::uint8_t>& SparsePointsStorageBackend::getValues() const
{
    return _values;
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

    const auto value = _values.data() + static_cast<std::size_t>(found - _indices.begin()) * _elementSize;
    switch (_elementTypeIndex)
    {
        case 0:
            return readValueAsFloat<float>(value);

        case 1:
            return readValueAsFloat<biovault::bfloat16_t>(value);

        case 2:
            return readValueAsFloat<std::int32_t>(value);

        case 3:
            return readValueAsFloat<std::uint32_t>(value);

        case 4:
            return readValueAsFloat<std::int16_t>(value);

        case 5:
            return readValueAsFloat<std::uint16_t>(value);

        case 6:
            return readValueAsFloat<std::int8_t>(value);

        case 7:
            return readValueAsFloat<std::uint8_t>(value);

        default:
            return 0.0f;
    }
}

void SparsePointsStorageBackend::copyRowRange(const std::uint64_t row, const std::uint64_t firstColumn, const std::uint64_t columnCount, std::uint8_t* destination) const
{
    if (_layout == SparseMatrixLayout::RowCompressed)
    {
        const auto begin        = _pointers[static_cast<std::size_t>(row)];
        const auto end          = _pointers[static_cast<std::size_t>(row + 1)];
        const auto first        = _indices.begin() + static_cast<std::ptrdiff_t>(begin);
        const auto last         = _indices.begin() + static_cast<std::ptrdiff_t>(end);
        const auto rangeBegin   = std::lower_bound(first, last, firstColumn);
        const auto rangeEnd     = std::lower_bound(rangeBegin, last, firstColumn + columnCount);

        for (auto index = rangeBegin; index != rangeEnd; ++index)
        {
            const auto valueIndex = static_cast<std::size_t>(index - _indices.begin());
            std::memcpy(destination + (*index - firstColumn) * _elementSize, _values.data() + valueIndex * _elementSize, _elementSize);
        }

        return;
    }

    for (auto column = firstColumn; column < firstColumn + columnCount; ++column)
    {
        const auto compressedIndex = column;
        const auto targetIndex     = row;
        const auto begin            = _pointers[static_cast<std::size_t>(compressedIndex)];
        const auto end              = _pointers[static_cast<std::size_t>(compressedIndex + 1)];
        const auto first            = _indices.begin() + static_cast<std::ptrdiff_t>(begin);
        const auto last             = _indices.begin() + static_cast<std::ptrdiff_t>(end);
        const auto found            = std::lower_bound(first, last, targetIndex);
        if (found != last && *found == targetIndex)
        {
            const auto valueIndex = static_cast<std::size_t>(found - _indices.begin());
            std::memcpy(destination + (column - firstColumn) * _elementSize, _values.data() + valueIndex * _elementSize, _elementSize);
        }
    }
}

}
