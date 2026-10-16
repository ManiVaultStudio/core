// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "TiledPointsStorageBackend.h"

#include "StorageBackendConversionAction.h"

#include <algorithm>
#include <cstring>
#include <stdexcept>

namespace mv::pointdata::storage
{

namespace
{

PointDataAccessView makeFloatAccessView(std::vector<float>&& values)
{
    auto bytes = std::make_shared<QByteArray>(reinterpret_cast<const char*>(values.data()), static_cast<qsizetype>(values.size() * sizeof(float)));
    PointDataAccessView view;
    view.data              = bytes->constData();
    view.rows              = 1;
    view.columns           = static_cast<std::uint64_t>(values.size());
    view.rowStrideBytes    = static_cast<std::uint64_t>(bytes->size());
    view.elementTypeIndex  = 0;
    view.elementSizeBytes  = sizeof(float);
    view.lifetime           = std::shared_ptr<const void>(bytes, bytes->constData());
    return view;
}

PointDataAccessView makeTileAccessView(const mv::pointdata::tiled::TileView& tile, const std::uint64_t rowOffset, const std::uint64_t columnOffset, const std::uint64_t rows, const std::uint64_t columns, const std::uint32_t typeIndex)
{
    PointDataAccessView view;
    view.data = static_cast<const char*>(tile.data())
        + rowOffset * tile.columns() * tile.elementSize()
        + columnOffset * tile.elementSize();
    view.rows             = rows;
    view.columns          = columns;
    view.rowStrideBytes   = tile.columns() * tile.elementSize();
    view.elementTypeIndex = typeIndex;
    view.elementSizeBytes = static_cast<std::uint32_t>(tile.elementSize());
    view.lifetime         = tile.lifetimeToken();
    return view;
}

}

TiledPointsStorageBackend::TiledPointsStorageBackend(const std::shared_ptr<mv::pointdata::tiled::PointDataTiledStorage>& storage, const std::uint32_t elementTypeIndex) :
    _storage(storage),
    _elementTypeIndex(elementTypeIndex)
{
}

StorageBackendType TiledPointsStorageBackend::getType() const
{
    return StorageBackendType::Tiled;
}

StorageBackendCapabilities TiledPointsStorageBackend::getCapabilities() const
{
    return StorageBackendCapability::RandomAccess | StorageBackendCapability::RangeAccess | StorageBackendCapability::Lazy | StorageBackendCapability::Materializable;
}

std::unique_ptr<mv::gui::StorageBackendConversionAction> TiledPointsStorageBackend::createConversionAction(const StorageBackendType, QObject*) const
{
    return {};
}

std::uint64_t TiledPointsStorageBackend::getNumPoints() const
{
    return _storage ? _storage->rows() : 0;
}

std::uint64_t TiledPointsStorageBackend::getNumDimensions() const
{
    return _storage ? _storage->columns() : 0;
}

std::size_t TiledPointsStorageBackend::getElementSize() const
{
    return _storage ? _storage->elementSize() : 0;
}

std::uint32_t TiledPointsStorageBackend::getElementTypeIndex() const
{
    return _elementTypeIndex;
}

PointDataAccessView TiledPointsStorageBackend::getRow(const std::uint64_t row) const
{
    if (!_storage || row >= _storage->rows())
        return {};

    std::vector<float> values(static_cast<std::size_t>(_storage->columns()));

    for (std::uint64_t column = 0; column < _storage->columns(); ++column)
        values[static_cast<std::size_t>(column)] = getValueAt(row * _storage->columns() + column);

    return makeFloatAccessView(std::move(values));
}

void TiledPointsStorageBackend::visitRange(const std::uint64_t firstRow, const std::uint64_t rowCount, const std::uint64_t firstColumn, const std::uint64_t columnCount, const PointDataAccessVisitor& visitor) const
{
    if (!_storage || !visitor || firstRow > getNumPoints() || rowCount > getNumPoints() - firstRow || firstColumn > getNumDimensions() || columnCount > getNumDimensions() - firstColumn)
        throw std::out_of_range("Point-data range is invalid");

    if (rowCount == 0 || columnCount == 0)
        return;

    for (std::uint64_t row = firstRow; row < firstRow + rowCount; ++row)
    {
        std::uint64_t column = firstColumn;
        const auto endColumn = firstColumn + columnCount;

        while (column < endColumn)
        {
            const auto tile = _storage->acquire(row / _storage->rowTileSize(), column / _storage->columnTileSize());
            const auto segmentEnd = std::min(endColumn, tile.columnOffset() + tile.columns());

            visitor(makeTileAccessView(tile, row - tile.rowOffset(), column - tile.columnOffset(), 1, segmentEnd - column, _elementTypeIndex));
            column = segmentEnd;
        }
    }
}

float TiledPointsStorageBackend::getValueAt(const std::uint64_t flatIndex) const
{
    return _storage ? _storage->valueAt(flatIndex, _elementTypeIndex) : 0.0f;
}

void TiledPointsStorageBackend::materialize(char* destination) const
{
    if (_storage && destination != nullptr)
        _storage->materialize(destination);
}

}
