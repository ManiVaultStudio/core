// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "DensePointsStorageBackend.h"

#include "../../PointData.h"
#include "DenseStorageBackendConversionAction.h"
#include "../interface/StorageBackendConversionAction.h"
#include "../sparse/SparseStorageBackendConversionAction.h"
#include "../tiled/TiledStorageBackendConversionAction.h"

#include <algorithm>
#include <cstring>
#include <stdexcept>

namespace mv::pointdata::storage
{

DensePointsStorageBackend::DensePointsStorageBackend(const ::PointData& pointData) :
    _pointData(pointData)
{
}

StorageBackendType DensePointsStorageBackend::getType() const
{
    return StorageBackendType::Dense;
}

StorageBackendCapabilities DensePointsStorageBackend::getCapabilities() const
{
    return getStorageBackendCapabilities(StorageBackendType::Dense);
}

std::unique_ptr<mv::gui::StorageBackendConversionAction> DensePointsStorageBackend::createConversionAction(const StorageBackendType targetType, QObject* parent) const
{
    const auto elementTypeIndex = getElementTypeIndex();

    if (targetType == StorageBackendType::Dense)
        return std::make_unique<mv::gui::DenseStorageBackendConversionAction>(parent, _pointData.getNumPoints(), _pointData.getNumDimensions(), elementTypeIndex);

    if (targetType == StorageBackendType::Tiled)
        return std::make_unique<mv::gui::TiledStorageBackendConversionAction>(parent, _pointData.getNumPoints(), _pointData.getNumDimensions(), getElementSize(), pointdata::storage::StorageBackendConfiguration{}, false, true, elementTypeIndex);

    if (targetType == StorageBackendType::Sparse)
        return std::make_unique<mv::gui::SparseStorageBackendConversionAction>(parent, _pointData.getNumPoints(), _pointData.getNumDimensions(), elementTypeIndex);

    return {};
}

void DensePointsStorageBackend::applyConfiguration(const StorageBackendConfigurationOptionsPtr& options)
{
    if (options)
        throw std::invalid_argument("Dense point-data storage does not accept backend configuration");
}

std::uint64_t DensePointsStorageBackend::getNumPoints() const
{
    return _pointData.getNumPoints();
}

std::uint64_t DensePointsStorageBackend::getNumDimensions() const
{
    return _pointData.getNumDimensions();
}

std::size_t DensePointsStorageBackend::getElementSize() const
{
    return std::visit([](const auto& vector) { return sizeof(typename std::decay_t<decltype(vector)>::value_type); }, _pointData._variantOfVectors);
}

std::uint32_t DensePointsStorageBackend::getElementTypeIndex() const
{
    return static_cast<std::uint32_t>(_pointData._variantOfVectors.index());
}

std::uint64_t DensePointsStorageBackend::getStorageSizeBytes() const
{
    return std::visit([](const auto& vector) {
        return static_cast<std::uint64_t>(vector.size()) * sizeof(typename std::decay_t<decltype(vector)>::value_type);
    }, _pointData._variantOfVectors);
}

PointDataAccessView DensePointsStorageBackend::getRow(const std::uint64_t row) const
{
    return _pointData.getDenseRow(row);
}

void DensePointsStorageBackend::visitRange(const std::uint64_t firstRow, const std::uint64_t rowCount, const std::uint64_t firstColumn, const std::uint64_t columnCount, const PointDataAccessVisitor& visitor) const
{
    _pointData.visitDenseRange(firstRow, rowCount, firstColumn, columnCount, visitor);
}

float DensePointsStorageBackend::getValueAt(const std::uint64_t flatIndex) const
{
    return _pointData.getDenseValueAt(static_cast<std::size_t>(flatIndex));
}

void DensePointsStorageBackend::materialize(char* destination, StorageBackendProgressCallback progress) const
{
    std::visit([destination, &progress](const auto& vector) {
        using ValueType = typename std::decay_t<decltype(vector)>::value_type;
        const auto totalBytes = vector.size() * sizeof(ValueType);
        if (destination == nullptr && totalBytes != 0)
            throw std::invalid_argument("Dense materialization destination is null");
        if (progress)
            progress(0.0f);

        if (totalBytes == 0)
        {
            if (progress)
                progress(1.0f);
            return;
        }

        const auto chunkBytes = std::max<std::size_t>(64U * 1024U * 1024U, totalBytes / 100);
        for (std::size_t offset = 0; offset < totalBytes;)
        {
            const auto bytes = std::min(chunkBytes, totalBytes - offset);
            std::memcpy(destination + offset, reinterpret_cast<const char*>(vector.data()) + offset, bytes);
            offset += bytes;
            if (progress)
                progress(totalBytes == 0 ? 1.0f : static_cast<float>(offset) / static_cast<float>(totalBytes));
        }

        if (progress && totalBytes == 0)
            progress(1.0f);
    }, _pointData._variantOfVectors);
}

}
