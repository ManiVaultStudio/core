#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

/**
 * @brief Read-only storage-neutral view over point-data values.
 *
 * The view can refer to eager point data, a tiled payload, or a temporary
 * buffer. The lifetime token keeps the referenced storage alive while the
 * view is being consumed.
 *
 * @author Thomas Kroes
 */
struct PointDataAccessView
{
    const void* data = nullptr;
    std::uint64_t rows = 0;
    std::uint64_t columns = 0;
    std::uint64_t rowStrideBytes = 0;
    std::uint32_t elementTypeIndex = 0;
    std::uint32_t elementSizeBytes = 0;
    std::shared_ptr<const void> lifetime;

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
