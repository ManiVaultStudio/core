#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

/**
 * @brief Read-only storage-neutral view over point-data values.
 *
 * PointDataAccessView is the common value-passing type used by the row,
 * column, and range visitor APIs. It can refer to eager point data, a tiled
 * payload, or a temporary conversion buffer without exposing the backing
 * storage implementation to consumers. The view describes a row-major
 * rectangle and includes the element representation needed to interpret its
 * bytes.
 *
 * The lifetime token is intentionally separate from the data pointer. A
 * visitor may retain a copy of the view while it processes the callback, and
 * the token keeps the referenced tile or temporary buffer alive for that
 * period. The view is read-only; mutation must go through the existing
 * PointData mutation APIs, which may materialize tiled storage.
 *
 * @author Thomas Kroes
 */
struct PointDataAccessView
{
    const void*                  data             = nullptr; /**< Beginning of the read-only row-major byte range. */
    std::uint64_t                rows             = 0;       /**< Number of rows in the view. */
    std::uint64_t                columns          = 0;       /**< Number of columns in the view. */
    std::uint64_t                rowStrideBytes   = 0;       /**< Number of bytes between consecutive rows. */
    std::uint32_t                elementTypeIndex = 0;       /**< PointData element-type index used to interpret each value. */
    std::uint32_t                elementSizeBytes = 0;       /**< Size of one value in bytes. */
    std::shared_ptr<const void> lifetime;                    /**< Ownership token that keeps the backing payload alive. */

    /** Returns whether the view contains a non-empty value range. */
    explicit operator bool() const
    {
        return data != nullptr && rows != 0 && columns != 0;
    }

    /** Returns a pointer to the first byte of the specified row. */
    const char* rowData(const std::uint64_t row) const
    {
        return static_cast<const char*>(data) + row * rowStrideBytes;
    }
};

/** Callback used by the storage-neutral point-data visiting functions. */
using PointDataAccessVisitor = std::function<void(const PointDataAccessView&)>;
