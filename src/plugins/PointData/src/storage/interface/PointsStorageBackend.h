// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "PointDataAccess.h"
#include "PointsStorageBackendTypes.h"

#include <pointdata_export.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>
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
 * subsets and proxy datasets. Read operations can be implemented by eager
 * and lazy representations alike; optional write operations fail explicitly
 * until a backend implements them.
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

    /** Creates the controls used to configure conversion to \p targetType. */
    virtual std::unique_ptr<mv::gui::StorageBackendConversionAction> createConversionAction(StorageBackendType targetType, QObject* parent) const = 0;

    /**
     * Applies backend-owned configuration options to the current representation.
     *
     * The options object must be compatible with this backend. Implementations
     * reject unsupported or invalid options rather than silently ignoring them.
     * Applying options may change runtime settings, such as cache limits,
     * without changing the backend type or materializing the representation.
     */
    virtual void applyConfiguration(const StorageBackendConfigurationOptionsPtr& configurationOptions) = 0;

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

    /**
     * Sets one row-major element from a typed one-element view when supported.
     *
     * Backends without element mutation throw std::logic_error. A writable
     * backend can override this without changing the read-only access API.
     */
    virtual void setValueAt(std::uint64_t, const PointDataAccessView&)
    {
        throw std::logic_error("Point-data storage does not support element mutation");
    }

    /**
     * Sets a rectangular range from a row-major view when supported.
     *
     * The view describes the source element type and row stride. Its payload
     * must remain valid for the duration of the call. Backends without range
     * mutation throw std::logic_error.
     */
    virtual void setRange(std::uint64_t, std::uint64_t, std::uint64_t, std::uint64_t, const PointDataAccessView&)
    {
        throw std::logic_error("Point-data storage does not support range mutation");
    }

    /** Materializes the complete matrix into a row-major destination buffer. */
    virtual void materialize(char* destination) const = 0;
};

}
