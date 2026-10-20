// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "../interface/PointsStorageBackend.h"

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

    /** Returns the eager backend type. */
    StorageBackendType getType() const override;

    /** Returns the eager backend capabilities, including materialization. */
    StorageBackendCapabilities getCapabilities() const override;

    /** Creates conversion controls for a supported target backend. */
    std::unique_ptr<mv::gui::StorageBackendConversionAction> createConversionAction(StorageBackendType targetType, QObject* parent) const override;
    /** Rejects configuration because eager storage has no backend-owned options yet. */
    void applyConfiguration(const StorageBackendConversionOptionsPtr& options) override;
    /** Returns the number of rows in the referenced PointData object. */
    std::uint64_t getNumPoints() const override;

    /** Returns the number of dimensions in the referenced PointData object. */
    std::uint64_t getNumDimensions() const override;

    /** Returns the size of one stored element in bytes. */
    std::size_t getElementSize() const override;

    /** Returns the PointData element-type index used by access views. */
    std::uint32_t getElementTypeIndex() const override;

    /** Returns one row as a lifetime-safe read-only view. */
    PointDataAccessView getRow(std::uint64_t row) const override;

    /** Visits a bounded row-major range in the referenced eager data. */
    void visitRange(std::uint64_t firstRow, std::uint64_t rowCount, std::uint64_t firstColumn, std::uint64_t columnCount, const PointDataAccessVisitor& visitor) const override;

    /** Reads one row-major element and converts it to float. */
    float getValueAt(std::uint64_t flatIndex) const override;

    /** Copies the complete eager matrix to the caller-owned destination buffer. */
    void materialize(char* destination) const override;

private:
    const ::PointData& _pointData; /**< Existing eager PointData owner. */
};

}
}
