// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "EagerPointsStorageBackend.h"

#include "../PointData.h"

#include <cstring>

namespace mv::pointdata::storage
{

EagerPointsStorageBackend::EagerPointsStorageBackend(const ::PointData& pointData) :
    _pointData(pointData)
{
}

StorageBackendType EagerPointsStorageBackend::getType() const
{
    return StorageBackendType::Eager;
}

StorageBackendCapabilities EagerPointsStorageBackend::getCapabilities() const
{
    return StorageBackendCapability::RandomAccess | StorageBackendCapability::RangeAccess | StorageBackendCapability::Writable | StorageBackendCapability::Materializable;
}

std::uint64_t EagerPointsStorageBackend::getNumPoints() const
{
    return _pointData.getNumPoints();
}

std::uint64_t EagerPointsStorageBackend::getNumDimensions() const
{
    return _pointData.getNumDimensions();
}

std::size_t EagerPointsStorageBackend::getElementSize() const
{
    return std::visit([](const auto& vector) { return sizeof(typename std::decay_t<decltype(vector)>::value_type); }, _pointData._variantOfVectors);
}

std::uint32_t EagerPointsStorageBackend::getElementTypeIndex() const
{
    return static_cast<std::uint32_t>(_pointData._variantOfVectors.index());
}

PointDataAccessView EagerPointsStorageBackend::getRow(const std::uint64_t row) const
{
    return _pointData.getEagerRow(row);
}

void EagerPointsStorageBackend::visitRange(const std::uint64_t firstRow, const std::uint64_t rowCount, const std::uint64_t firstColumn, const std::uint64_t columnCount, const PointDataAccessVisitor& visitor) const
{
    _pointData.visitEagerRange(firstRow, rowCount, firstColumn, columnCount, visitor);
}

float EagerPointsStorageBackend::getValueAt(const std::uint64_t flatIndex) const
{
    return _pointData.getEagerValueAt(static_cast<std::size_t>(flatIndex));
}

void EagerPointsStorageBackend::materialize(char* destination) const
{
    if (destination == nullptr)
        return;

    std::visit([destination](const auto& vector) {
        if (!vector.empty())
            std::memcpy(destination, vector.data(), vector.size() * sizeof(typename std::decay_t<decltype(vector)>::value_type));
    }, _pointData._variantOfVectors);
}

}
