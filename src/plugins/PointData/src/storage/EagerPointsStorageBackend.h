// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "PointsStorageBackend.h"

class PointData;

namespace mv
{
namespace pointdata::storage
{

/**
 * @brief Transitional backend adapter for PointData's existing eager vectors.
 *
 * The adapter intentionally does not own the existing variant-of-vectors yet.
 * PointData remains the owner during this migration step, which preserves
 * legacy construction, mutation, and serialization behavior. The adapter
 * provides the backend contract around that storage so the vector ownership
 * can move here incrementally later without changing Points' public API.
 *
 * @author Thomas Kroes
 */
class POINTDATA_EXPORT EagerPointsStorageBackend final : public PointsStorageBackend
{
public:
    /**
     * Creates a non-owning adapter around \p pointData.
     *
     * The referenced PointData object must outlive this backend instance.
     */
    explicit EagerPointsStorageBackend(const ::PointData& pointData);

    StorageBackendType getType() const override;
    StorageBackendCapabilities getCapabilities() const override;
    std::unique_ptr<mv::gui::StorageBackendConversionAction> createConversionAction(StorageBackendType targetType, QObject* parent) const override;
    std::uint64_t getNumPoints() const override;
    std::uint64_t getNumDimensions() const override;
    std::size_t getElementSize() const override;
    std::uint32_t getElementTypeIndex() const override;
    PointDataAccessView getRow(std::uint64_t row) const override;
    void visitRange(std::uint64_t firstRow, std::uint64_t rowCount, std::uint64_t firstColumn, std::uint64_t columnCount, const PointDataAccessVisitor& visitor) const override;
    float getValueAt(std::uint64_t flatIndex) const override;
    void materialize(char* destination) const override;

private:
    const ::PointData& _pointData; /**< Existing eager PointData owner. */
};

}
}
