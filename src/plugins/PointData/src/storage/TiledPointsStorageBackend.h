// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "PointsStorageBackend.h"

#include "../tiled/PointDataTiledStorage.h"

#include <memory>

namespace mv::pointdata::storage
{

/**
 * @brief Storage-backend adapter for read-only rectangular tiled point data.
 *
 * The adapter exposes the existing tile index, bounded cache, and lifetime-safe
 * tile views through the common backend contract. It does not duplicate tile
 * ownership: PointDataTiledStorage remains responsible for descriptors,
 * loading, caching, and serialization.
 *
 * @author Thomas Kroes
 */
class POINTDATA_EXPORT TiledPointsStorageBackend final : public PointsStorageBackend
{
public:
    /**
     * Creates an adapter around \p storage and retains shared ownership of it.
     *
     * The element-type index is the PointData representation used when tile
     * bytes are converted to the facade's floating-point access API.
     */
    TiledPointsStorageBackend(const std::shared_ptr<mv::pointdata::tiled::PointDataTiledStorage>& storage, std::uint32_t elementTypeIndex);

    StorageBackendType getType() const override;
    StorageBackendCapabilities getCapabilities() const override;
    std::unique_ptr<mv::gui::StorageBackendConversionAction> createConversionAction(StorageBackendType targetType, QObject* parent) const override;
    /** Applies the tiled cache setting without changing tile geometry or backend type. */
    void applyConfiguration(const StorageBackendConversionOptionsPtr& options) override;
    std::uint64_t getNumPoints() const override;
    std::uint64_t getNumDimensions() const override;
    std::size_t getElementSize() const override;
    std::uint32_t getElementTypeIndex() const override;
    PointDataAccessView getRow(std::uint64_t row) const override;
    void visitRange(std::uint64_t firstRow, std::uint64_t rowCount, std::uint64_t firstColumn, std::uint64_t columnCount, const PointDataAccessVisitor& visitor) const override;
    float getValueAt(std::uint64_t flatIndex) const override;
    void materialize(char* destination) const override;

private:
    std::shared_ptr<mv::pointdata::tiled::PointDataTiledStorage> _storage; /**< Tiled payload and cache owner. */
    std::uint32_t                                               _elementTypeIndex = 0; /**< Stored PointData element type. */
};

}
