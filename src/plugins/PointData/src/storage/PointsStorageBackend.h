// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "../PointDataAccess.h"
#include "PointsStorageBackendTypes.h"

#include <pointdata_export.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

class QObject;

namespace mv::gui
{
    class StorageBackendConversionAction;
}

namespace mv::pointdata::storage
{

/**
 * @brief Storage-neutral access contract for one concrete point dataset.
 *
 * The backend owns the physical representation of a row-major point matrix.
 * PointData owns the backend and preserves the established eager APIs around
 * it, while Points remains responsible for logical facade behavior such as
 * subsets and proxy datasets. The contract deliberately contains only access
 * operations that can be implemented by eager and lazy representations alike.
 *
 * Mutation and backend-specific diagnostics are capability-dependent. A
 * read-only backend must not be forced to implement artificial write-back
 * behavior merely to satisfy this interface.
 *
 * @author Thomas Kroes
 */
class POINTDATA_EXPORT PointsStorageBackend
{
public:
    virtual ~PointsStorageBackend() = default;

    /** Returns the physical representation type. */
    virtual StorageBackendType getType() const = 0;

    /** Returns the operations supported by this backend. */
    virtual StorageBackendCapabilities getCapabilities() const = 0;

    /** Creates the controls used to configure conversion to p targetType. */
    virtual std::unique_ptr<mv::gui::StorageBackendConversionAction> createConversionAction(StorageBackendType targetType, QObject* parent) const = 0;

    /** Returns the number of matrix rows. */
    virtual std::uint64_t getNumPoints() const = 0;

    /** Returns the number of matrix columns. */
    virtual std::uint64_t getNumDimensions() const = 0;

    /** Returns the size of one stored element in bytes. */
    virtual std::size_t getElementSize() const = 0;

    /** Returns the stored element-type index used by PointDataAccessView. */
    virtual std::uint32_t getElementTypeIndex() const = 0;

    /** Returns one row as a lifetime-safe read-only view. */
    virtual PointDataAccessView getRow(std::uint64_t row) const = 0;

    /** Visits a rectangular matrix range in row-major order. */
    virtual void visitRange(std::uint64_t firstRow, std::uint64_t rowCount, std::uint64_t firstColumn, std::uint64_t columnCount, const PointDataAccessVisitor& visitor) const = 0;

    /** Reads one row-major element as a float. */
    virtual float getValueAt(std::uint64_t flatIndex) const = 0;

    /** Materializes the complete matrix into a row-major destination buffer. */
    virtual void materialize(char* destination) const = 0;
};

}
