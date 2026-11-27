// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2026 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "SparsePointsStorageBackend.h"

#include "../dense/DenseStorageBackendConversionAction.h"
#include "../interface/StorageBackendConversionAction.h"
#include "SparseStorageBackendConversionAction.h"
#include "../tiled/TiledStorageBackendConversionAction.h"

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

SparsePointsStorageBackend::SparsePointsStorageBackend(const std::uint64_t rows, const std::uint64_t columns, const std::uint32_t elementTypeIndex, const std::size_t elementSize,
    std::vector<std::uint64_t> rowPointers, std::vector<std::uint64_t> columnIndices, std::vector<std::uint8_t> rowValues,
    std::vector<std::uint64_t> columnPointers, std::vector<std::uint64_t> rowIndices, std::vector<std::uint8_t> columnValues) :
    SparsePointsStorageBackend(rows, columns, SparseMatrixLayout::RowCompressed, elementTypeIndex, elementSize,
        std::move(rowPointers), std::move(columnIndices), std::move(rowValues))
{
    SparsePointsStorageBackend validatedColumnIndex(rows, columns, SparseMatrixLayout::ColumnCompressed, elementTypeIndex, elementSize,
        std::move(columnPointers), std::move(rowIndices), std::move(columnValues));
    _secondaryPointers = std::move(validatedColumnIndex._pointers);
    _secondaryIndices = std::move(validatedColumnIndex._indices);
    _secondaryValues = std::move(validatedColumnIndex._values);
    if (_secondaryIndices.size() != _indices.size() || _secondaryValues.size() != _values.size())
        throw std::invalid_argument("CSR and CSC sparse indexes must contain the same number of values");
}

StorageBackendType SparsePointsStorageBackend::getType() const
{
    return StorageBackendType::Sparse;
}

StorageBackendCapabilities SparsePointsStorageBackend::getCapabilities() const
{
    return getStorageBackendCapabilities(StorageBackendType::Sparse);
}

std::unique_ptr<mv::gui::StorageBackendConversionAction> SparsePointsStorageBackend::createConversionAction(const StorageBackendType targetType, QObject* parent) const
{
    const auto elementTypeIndex = getElementTypeIndex();

    if (targetType == StorageBackendType::Sparse)
        return std::make_unique<mv::gui::SparseStorageBackendConversionAction>(parent, _rows, _columns, elementTypeIndex,
            _layout == SparseMatrixLayout::ColumnCompressed, 4, 128, hasRowCompressedIndex(), hasColumnCompressedIndex());

    if (targetType == StorageBackendType::Tiled)
        return std::make_unique<mv::gui::TiledStorageBackendConversionAction>(parent, _rows, _columns, _elementSize, StorageBackendConfiguration{}, false, true, elementTypeIndex);

    if (targetType == StorageBackendType::Dense)
        return std::make_unique<mv::gui::DenseStorageBackendConversionAction>(parent, _rows, _columns, elementTypeIndex);

    return {};
}

void SparsePointsStorageBackend::applyConfiguration(const StorageBackendConfigurationOptionsPtr& options)
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

void SparsePointsStorageBackend::visitColumns(const std::vector<std::uint64_t>& columnIds, const std::vector<std::uint64_t>& rowIds, const PointDataAccessVisitor& visitor) const
{
    if (!visitor)
        throw std::invalid_argument("Point-data visitor is invalid");
    for (const auto column : columnIds)
        if (column >= _columns)
            throw std::out_of_range("Point-data column ID is out of bounds");
    for (const auto row : rowIds)
        if (row >= _rows)
            throw std::out_of_range("Point-data row ID is out of bounds");
    if (columnIds.empty())
        return;

    const auto& pointers = hasColumnCompressedIndex() ? getColumnPointers() : _pointers;
    const auto& indices = hasColumnCompressedIndex() ? getRowIndices() : _indices;
    const auto& values = hasColumnCompressedIndex() ? getColumnValues() : _values;
    const auto columnCompressed = hasColumnCompressedIndex();
    for (const auto row : rowIds)
    {
        for (std::size_t first = 0; first < columnIds.size();)
        {
            auto end = first + 1;
            while (end < columnIds.size() && columnIds[end - 1] != std::numeric_limits<std::uint64_t>::max() && columnIds[end] == columnIds[end - 1] + 1)
                ++end;
            const auto count = end - first;
            if (count > std::numeric_limits<std::size_t>::max() / _elementSize)
                throw std::overflow_error("Sparse selected columns exceed the process address space");
            auto output = std::make_shared<std::vector<std::uint8_t>>(count * _elementSize, 0);
            for (std::size_t outputColumn = 0; outputColumn < count; ++outputColumn)
            {
                const auto column = columnIds[first + outputColumn];
                if (columnCompressed)
                {
                    const auto begin = pointers[static_cast<std::size_t>(column)];
                    const auto segmentEnd = pointers[static_cast<std::size_t>(column + 1)];
                    const auto segment = indices.begin() + static_cast<std::ptrdiff_t>(begin);
                    const auto found = std::lower_bound(segment, indices.begin() + static_cast<std::ptrdiff_t>(segmentEnd), row);
                    if (found != indices.begin() + static_cast<std::ptrdiff_t>(segmentEnd) && *found == row)
                        std::memcpy(output->data() + outputColumn * _elementSize, values.data() + static_cast<std::size_t>(found - indices.begin()) * _elementSize, _elementSize);
                }
                else
                    copyRowRange(row, column, 1, output->data() + outputColumn * _elementSize);
            }
            PointDataAccessView view;
            view.data = output->data();
            view.rows = 1;
            view.columns = count;
            view.rowStrideBytes = count * _elementSize;
            view.elementTypeIndex = getElementTypeIndex();
            view.elementSizeBytes = static_cast<std::uint32_t>(_elementSize);
            view.lifetime = std::shared_ptr<const void>(output, output->data());
            visitor(view);
            first = end;
        }
    }
}

float SparsePointsStorageBackend::getValueAt(const std::uint64_t flatIndex) const
{
    if (_columns == 0 || (_rows != 0 && _columns > std::numeric_limits<std::uint64_t>::max() / _rows) || flatIndex >= _rows * _columns)
        throw std::out_of_range("Sparse point-data index is invalid");

    return valueAt(flatIndex / _columns, flatIndex % _columns);
}

void SparsePointsStorageBackend::materialize(char* destination, StorageBackendProgressCallback progress) const
{
    if (progress)
        progress(0.0f);

    if (_rows != 0 && _columns > std::numeric_limits<std::uint64_t>::max() / _rows)
        throw std::overflow_error("Sparse matrix dimensions overflow the dense element count");

    const auto elementCount = _rows * _columns;
    if (_elementSize == 0 || elementCount > std::numeric_limits<std::size_t>::max() / _elementSize)
        throw std::overflow_error("Sparse matrix is too large to materialize");

    const auto totalBytes = static_cast<std::size_t>(elementCount) * _elementSize;
    if (!destination && totalBytes != 0)
        throw std::invalid_argument("Sparse materialization destination is null");
    const auto zeroChunkBytes = std::max<std::size_t>(64U * 1024U * 1024U, totalBytes / 100);
    for (std::size_t offset = 0; offset < totalBytes;)
    {
        const auto bytes = std::min(zeroChunkBytes, totalBytes - offset);
        std::memset(destination + offset, 0, bytes);
        offset += bytes;
        if (progress)
            progress(totalBytes == 0 ? 0.1f : 0.1f * static_cast<float>(offset) / static_cast<float>(totalBytes));
    }
    if (progress && totalBytes == 0)
        progress(0.1f);

    if (_layout == SparseMatrixLayout::RowCompressed)
    {
        const auto progressInterval = std::max<std::uint64_t>(1, _rows / 100);
        for (std::uint64_t row = 0; row < _rows; ++row)
        {
            for (auto index = _pointers[static_cast<std::size_t>(row)]; index < _pointers[static_cast<std::size_t>(row + 1)]; ++index)
            {
                const auto column = _indices[static_cast<std::size_t>(index)];
                const auto flatIndex = row * _columns + column;
                std::memcpy(destination + flatIndex * _elementSize, _values.data() + static_cast<std::size_t>(index) * _elementSize, _elementSize);
            }
            if (progress && (row == 0 || row + 1 == _rows || (row + 1) % progressInterval == 0))
                progress(0.1f + 0.9f * static_cast<float>(row + 1) / static_cast<float>(_rows));
        }
    }
    else
    {
        const auto progressInterval = std::max<std::uint64_t>(1, _columns / 100);
        for (std::uint64_t column = 0; column < _columns; ++column)
        {
            for (auto index = _pointers[static_cast<std::size_t>(column)]; index < _pointers[static_cast<std::size_t>(column + 1)]; ++index)
            {
                const auto row = _indices[static_cast<std::size_t>(index)];
                const auto flatIndex = row * _columns + column;
                std::memcpy(destination + flatIndex * _elementSize, _values.data() + static_cast<std::size_t>(index) * _elementSize, _elementSize);
            }
            if (progress && (column == 0 || column + 1 == _columns || (column + 1) % progressInterval == 0))
                progress(0.1f + 0.9f * static_cast<float>(column + 1) / static_cast<float>(_columns));
        }
    }

    if (progress && ((_layout == SparseMatrixLayout::RowCompressed && _rows == 0) || (_layout == SparseMatrixLayout::ColumnCompressed && _columns == 0)))
        progress(1.0f);
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
    return (static_cast<std::uint64_t>(_pointers.size()) + _secondaryPointers.size()) * sizeof(std::uint64_t) +
        (static_cast<std::uint64_t>(_indices.size()) + _secondaryIndices.size()) * sizeof(std::uint64_t) +
        static_cast<std::uint64_t>(_values.size()) + _secondaryValues.size();
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

bool SparsePointsStorageBackend::hasRowCompressedIndex() const { return _layout == SparseMatrixLayout::RowCompressed || (_secondaryLayout == SparseMatrixLayout::RowCompressed && !_secondaryPointers.empty()); }
bool SparsePointsStorageBackend::hasColumnCompressedIndex() const { return _layout == SparseMatrixLayout::ColumnCompressed || (_secondaryLayout == SparseMatrixLayout::ColumnCompressed && !_secondaryPointers.empty()); }
const std::vector<std::uint64_t>& SparsePointsStorageBackend::getRowPointers() const { return _layout == SparseMatrixLayout::RowCompressed ? _pointers : _secondaryPointers; }
const std::vector<std::uint64_t>& SparsePointsStorageBackend::getColumnIndices() const { return _layout == SparseMatrixLayout::RowCompressed ? _indices : _secondaryIndices; }
const std::vector<std::uint8_t>& SparsePointsStorageBackend::getRowValues() const { return _layout == SparseMatrixLayout::RowCompressed ? _values : _secondaryValues; }
const std::vector<std::uint64_t>& SparsePointsStorageBackend::getColumnPointers() const { return _layout == SparseMatrixLayout::ColumnCompressed ? _pointers : _secondaryPointers; }
const std::vector<std::uint64_t>& SparsePointsStorageBackend::getRowIndices() const { return _layout == SparseMatrixLayout::ColumnCompressed ? _indices : _secondaryIndices; }
const std::vector<std::uint8_t>& SparsePointsStorageBackend::getColumnValues() const { return _layout == SparseMatrixLayout::ColumnCompressed ? _values : _secondaryValues; }

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
