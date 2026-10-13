#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

/** Read-only storage-neutral view over point-data values. */
struct PointDataAccessView
{
    const void* data = nullptr;
    std::uint64_t rows = 0;
    std::uint64_t columns = 0;
    std::uint64_t rowStrideBytes = 0;
    std::uint32_t elementTypeIndex = 0;
    std::uint32_t elementSizeBytes = 0;
    std::shared_ptr<const void> lifetime;

    explicit operator bool() const { return data != nullptr && rows != 0 && columns != 0; }
    const char* rowData(const std::uint64_t row) const
    {
        return static_cast<const char*>(data) + row * rowStrideBytes;
    }
};

using PointDataAccessVisitor = std::function<void(const PointDataAccessView&)>;
