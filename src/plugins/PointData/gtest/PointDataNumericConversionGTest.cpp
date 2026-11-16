// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2026 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include <storage/interface/PointDataNumericConversion.h>
#include <storage/sparse/SparsePointsStorageBackend.h>
#include <storage/tiled/PointDataTiledStorage.h>
#include <storage/tiled/TiledPointsStorageBackend.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace
{
using mv::pointdata::storage::detail::convertNumericRow;

constexpr std::uint32_t numericTypeCount = 8;
constexpr std::size_t conversionValueCount = 9 * 17;
constexpr std::array<long double, 20> conversionProbes{ -40000.75L, -32768.0L, -257.0L, -128.0L, -2.75L, -1.5L, -0.5L, 0.0L, 0.5L, 1.5L, 2.75L, 127.0L, 128.0L, 255.0L, 256.0L, 32767.0L, 32768.0L, 65535.0L, 65536.0L, 4294967296.0L };
constexpr std::array<std::size_t, numericTypeCount> typeSizes{ sizeof(float), sizeof(biovault::bfloat16_t), sizeof(std::int32_t), sizeof(std::uint32_t), sizeof(std::int16_t), sizeof(std::uint16_t), sizeof(std::int8_t), sizeof(std::uint8_t) };
constexpr std::uint64_t conversionRows = 9;
constexpr std::uint64_t conversionColumns = 17;

bool expectedConversion(const long double source, const std::uint32_t targetType, long double& expected)
{
    switch (targetType)
    {
        case 0: expected = static_cast<float>(source); return true;
        case 1: expected = static_cast<float>(biovault::bfloat16_t(static_cast<float>(source))); return true;
        case 2: if (!std::isfinite(source) || source < std::numeric_limits<std::int32_t>::lowest() || source > std::numeric_limits<std::int32_t>::max()) return false; expected = static_cast<std::int32_t>(source); return true;
        case 3: if (!std::isfinite(source) || source < 0 || source > std::numeric_limits<std::uint32_t>::max()) return false; expected = static_cast<std::uint32_t>(source); return true;
        case 4: if (!std::isfinite(source) || source < std::numeric_limits<std::int16_t>::lowest() || source > std::numeric_limits<std::int16_t>::max()) return false; expected = static_cast<std::int16_t>(source); return true;
        case 5: if (!std::isfinite(source) || source < 0 || source > std::numeric_limits<std::uint16_t>::max()) return false; expected = static_cast<std::uint16_t>(source); return true;
        case 6: if (!std::isfinite(source) || source < std::numeric_limits<std::int8_t>::lowest() || source > std::numeric_limits<std::int8_t>::max()) return false; expected = static_cast<std::int8_t>(source); return true;
        case 7: if (!std::isfinite(source) || source < 0 || source > std::numeric_limits<std::uint8_t>::max()) return false; expected = static_cast<std::uint8_t>(source); return true;
        default: throw std::invalid_argument("Unsupported conversion target type");
    }
}

struct HeadlessPointMatrix
{
    mv::pointdata::storage::StorageBackendType backendType;
    std::uint32_t elementTypeIndex;
    std::vector<std::uint8_t> denseData;
    std::shared_ptr<mv::pointdata::tiled::PointDataTiledStorage> tiledStorage;
    std::unique_ptr<mv::pointdata::storage::SparsePointsStorageBackend> sparseStorage;

    std::vector<std::uint8_t> getRow(const std::uint64_t row) const
    {
        const auto rowBytes = static_cast<std::size_t>(conversionColumns) * typeSizes[elementTypeIndex];
        if (backendType == mv::pointdata::storage::StorageBackendType::Dense)
        {
            std::vector<std::uint8_t> values(rowBytes);
            std::memcpy(values.data(), denseData.data() + static_cast<std::size_t>(row) * rowBytes, rowBytes);
            return values;
        }

        const auto view = backendType == mv::pointdata::storage::StorageBackendType::Tiled
            ? mv::pointdata::storage::TiledPointsStorageBackend(tiledStorage, elementTypeIndex).getRow(row)
            : sparseStorage->getRow(row);
        EXPECT_NE(view.data, nullptr);
        EXPECT_EQ(view.columns, conversionColumns);
        EXPECT_EQ(view.elementTypeIndex, elementTypeIndex);

        std::vector<std::uint8_t> values(rowBytes);
        std::memcpy(values.data(), view.data, rowBytes);
        return values;
    }
};

bool isNonZero(const std::uint32_t typeIndex, const std::uint8_t* bytes)
{
    switch (typeIndex)
    {
        case 0: { float value; std::memcpy(&value, bytes, sizeof(value)); return value != 0.0f; }
        case 1: { biovault::bfloat16_t value; std::memcpy(&value, bytes, sizeof(value)); return static_cast<float>(value) != 0.0f; }
        case 2: { std::int32_t value; std::memcpy(&value, bytes, sizeof(value)); return value != 0; }
        case 3: { std::uint32_t value; std::memcpy(&value, bytes, sizeof(value)); return value != 0; }
        case 4: { std::int16_t value; std::memcpy(&value, bytes, sizeof(value)); return value != 0; }
        case 5: { std::uint16_t value; std::memcpy(&value, bytes, sizeof(value)); return value != 0; }
        case 6: { std::int8_t value; std::memcpy(&value, bytes, sizeof(value)); return value != 0; }
        case 7: { std::uint8_t value; std::memcpy(&value, bytes, sizeof(value)); return value != 0; }
        default: throw std::invalid_argument("Unsupported point matrix element type");
    }
}

HeadlessPointMatrix makeBackendMatrix(const mv::pointdata::storage::StorageBackendType backendType, const std::uint32_t elementTypeIndex, std::vector<std::uint8_t> denseValues)
{
    HeadlessPointMatrix matrix{ backendType, elementTypeIndex };
    if (backendType == mv::pointdata::storage::StorageBackendType::Dense)
    {
        matrix.denseData = std::move(denseValues);
        return matrix;
    }

    if (backendType == mv::pointdata::storage::StorageBackendType::Tiled)
    {
        matrix.tiledStorage = mv::pointdata::tiled::PointDataTiledStorage::fromDense(reinterpret_cast<const char*>(denseValues.data()), conversionRows, conversionColumns, typeSizes[elementTypeIndex], 4, 8);
        return matrix;
    }

    std::vector<std::uint64_t> rowPointers(static_cast<std::size_t>(conversionRows) + 1, 0);
    std::vector<std::uint64_t> columnIndices;
    std::vector<std::uint8_t> sparseValues;
    for (std::uint64_t row = 0; row < conversionRows; ++row)
    {
        for (std::uint64_t column = 0; column < conversionColumns; ++column)
        {
            const auto* value = denseValues.data() + static_cast<std::size_t>(row * conversionColumns + column) * typeSizes[elementTypeIndex];
            if (!isNonZero(elementTypeIndex, value))
                continue;

            columnIndices.push_back(column);
            sparseValues.insert(sparseValues.end(), value, value + typeSizes[elementTypeIndex]);
        }
        rowPointers[static_cast<std::size_t>(row + 1)] = columnIndices.size();
    }

    matrix.sparseStorage = std::make_unique<mv::pointdata::storage::SparsePointsStorageBackend>(conversionRows, conversionColumns, mv::pointdata::storage::SparseMatrixLayout::RowCompressed, elementTypeIndex, typeSizes[elementTypeIndex], std::move(rowPointers), std::move(columnIndices), std::move(sparseValues));
    return matrix;
}

HeadlessPointMatrix convertBackendMatrix(const HeadlessPointMatrix& source, const mv::pointdata::storage::StorageBackendType targetBackend, const std::uint32_t targetTypeIndex)
{
    if (targetBackend == mv::pointdata::storage::StorageBackendType::Tiled)
    {
        HeadlessPointMatrix matrix{ targetBackend, targetTypeIndex };
        const auto sourceElementSize = typeSizes[source.elementTypeIndex];
        const auto targetElementSize = typeSizes[targetTypeIndex];
        matrix.tiledStorage = mv::pointdata::tiled::PointDataTiledStorage::fromGenerator(conversionRows, conversionColumns, targetElementSize, 4, 8, [&source, sourceElementSize, targetElementSize, targetTypeIndex](const mv::pointdata::tiled::TileDescriptor& descriptor) {
            const auto rowBytes = static_cast<std::size_t>(descriptor.columns) * targetElementSize;
            const auto payloadBytes = static_cast<std::size_t>(descriptor.rows) * rowBytes;
            QByteArray payload(static_cast<qsizetype>(payloadBytes), Qt::Uninitialized);
            std::vector<std::uint8_t> convertedRow(rowBytes);
            for (std::uint64_t localRow = 0; localRow < descriptor.rows; ++localRow)
            {
                const auto sourceRow = source.getRow(descriptor.rowOffset + localRow);
                const auto* sourceSegment = sourceRow.data() + descriptor.columnOffset * sourceElementSize;
                convertNumericRow(source.elementTypeIndex, targetTypeIndex, reinterpret_cast<const char*>(sourceSegment), descriptor.columns, convertedRow.data());
                if (rowBytes != 0)
                    std::memcpy(payload.data() + localRow * rowBytes, convertedRow.data(), rowBytes);
            }

            return payload;
        }, {}, true);
        return matrix;
    }

    std::vector<std::uint8_t> converted(static_cast<std::size_t>(conversionRows * conversionColumns) * typeSizes[targetTypeIndex]);
    std::vector<std::uint8_t> convertedRow(static_cast<std::size_t>(conversionColumns) * typeSizes[targetTypeIndex]);
    for (std::uint64_t row = 0; row < conversionRows; ++row)
    {
        const auto sourceRow = source.getRow(row);
        convertNumericRow(source.elementTypeIndex, targetTypeIndex, reinterpret_cast<const char*>(sourceRow.data()), conversionColumns, convertedRow.data());
        std::memcpy(converted.data() + static_cast<std::size_t>(row * conversionColumns) * typeSizes[targetTypeIndex], convertedRow.data(), convertedRow.size());
    }

    return makeBackendMatrix(targetBackend, targetTypeIndex, std::move(converted));
}

template <typename T>
long double toLongDouble(const T value)
{
    if constexpr (std::is_same_v<T, biovault::bfloat16_t>)
        return static_cast<float>(value);
    else
        return static_cast<long double>(value);
}

template <typename T>
std::vector<T> makeConversionValues()
{
    std::vector<T> values;
    for (const auto probe : conversionProbes)
    {
        if constexpr (std::is_integral_v<T>)
        {
            if (probe < static_cast<long double>(std::numeric_limits<T>::lowest()) || probe > static_cast<long double>(std::numeric_limits<T>::max()))
                continue;
        }

        const auto value = static_cast<T>(probe);
        if (std::find(values.cbegin(), values.cend(), value) == values.cend())
            values.push_back(value);
    }

    if constexpr (std::is_integral_v<T>)
    {
        for (const auto boundary : { std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max() })
        {
            if (std::find(values.cbegin(), values.cend(), boundary) == values.cend())
                values.push_back(boundary);
        }
    }

    if constexpr (std::is_floating_point_v<T>)
    {
        values.push_back(std::numeric_limits<T>::infinity());
        values.push_back(std::numeric_limits<T>::quiet_NaN());
    }

    return values;
}

template <>
std::vector<biovault::bfloat16_t> makeConversionValues<biovault::bfloat16_t>()
{
    const auto floats = makeConversionValues<float>();
    std::vector<biovault::bfloat16_t> values;
    values.reserve(floats.size());
    for (const auto value : floats)
        values.emplace_back(value);
    return values;
}

template <typename SourceType>
void runConversionCase(const mv::pointdata::storage::StorageBackendType sourceBackend, const mv::pointdata::storage::StorageBackendType targetBackend, const std::uint32_t sourceType, const std::uint32_t targetType, const bool targetSafeValues)
{
    auto sourceValues = makeConversionValues<SourceType>();
    if (targetSafeValues)
    {
        sourceValues.erase(std::remove_if(sourceValues.begin(), sourceValues.end(), [targetType](const auto value) {
            long double expected = 0;
            return !expectedConversion(toLongDouble(value), targetType, expected);
        }), sourceValues.end());
    }

    ASSERT_FALSE(sourceValues.empty());
    const auto initialValueCount = sourceValues.size();
    sourceValues.reserve(conversionValueCount);
    for (std::size_t index = 0; sourceValues.size() < conversionValueCount; ++index)
        sourceValues.push_back(sourceValues[index % initialValueCount]);

    std::vector<std::uint8_t> sourceBytes(sourceValues.size() * sizeof(SourceType));
    std::memcpy(sourceBytes.data(), sourceValues.data(), sourceBytes.size());

    auto sourceMatrix = makeBackendMatrix(sourceBackend, sourceType, sourceBytes);
    bool shouldFail = false;
    for (const auto value : sourceValues)
    {
        long double expected = 0;
        if (!expectedConversion(toLongDouble(value), targetType, expected))
            shouldFail = true;
    }

    bool conversionFailed = false;
    std::optional<HeadlessPointMatrix> convertedMatrix;
    try
    {
        convertedMatrix.emplace(convertBackendMatrix(sourceMatrix, targetBackend, targetType));
    }
    catch (const std::range_error&)
    {
        conversionFailed = true;
    }

    EXPECT_EQ(conversionFailed, shouldFail);
    if (conversionFailed)
    {
        EXPECT_EQ(sourceMatrix.backendType, sourceBackend);
        EXPECT_EQ(sourceMatrix.elementTypeIndex, sourceType);
        for (std::uint64_t row = 0; row < conversionRows; ++row)
        {
            const auto actual = sourceMatrix.getRow(row);
            const auto offset = static_cast<std::size_t>(row * conversionColumns) * typeSizes[sourceType];
            EXPECT_TRUE(std::equal(actual.cbegin(), actual.cend(), sourceBytes.cbegin() + offset));
        }
        return;
    }

    ASSERT_TRUE(convertedMatrix.has_value());
    EXPECT_EQ(convertedMatrix->backendType, targetBackend);
    EXPECT_EQ(convertedMatrix->elementTypeIndex, targetType);

    for (std::uint64_t row = 0; row < conversionRows; ++row)
    {
        const auto actualRow = convertedMatrix->getRow(row);
        for (std::uint64_t column = 0; column < conversionColumns; ++column)
        {
            const auto valueIndex = static_cast<std::size_t>(row * conversionColumns + column);
            long double expected = 0;
            ASSERT_TRUE(expectedConversion(toLongDouble(sourceValues[valueIndex]), targetType, expected));

            const auto* bytes = actualRow.data() + static_cast<std::size_t>(column) * typeSizes[targetType];
            long double actual = 0;
            switch (targetType)
            {
                case 0: { float value; std::memcpy(&value, bytes, sizeof(value)); actual = value; break; }
                case 1: { biovault::bfloat16_t value; std::memcpy(&value, bytes, sizeof(value)); actual = static_cast<float>(value); break; }
                case 2: { std::int32_t value; std::memcpy(&value, bytes, sizeof(value)); actual = value; break; }
                case 3: { std::uint32_t value; std::memcpy(&value, bytes, sizeof(value)); actual = value; break; }
                case 4: { std::int16_t value; std::memcpy(&value, bytes, sizeof(value)); actual = value; break; }
                case 5: { std::uint16_t value; std::memcpy(&value, bytes, sizeof(value)); actual = value; break; }
                case 6: { std::int8_t value; std::memcpy(&value, bytes, sizeof(value)); actual = value; break; }
                case 7: { std::uint8_t value; std::memcpy(&value, bytes, sizeof(value)); actual = value; break; }
                default: FAIL() << "Unsupported target type index: " << targetType;
            }

            EXPECT_TRUE(actual == expected || (std::isnan(actual) && std::isnan(expected))) << "Converted value at row " << row << ", column " << column;
        }
    }
}

template <std::uint32_t SourceType>
void runSourceType(const mv::pointdata::storage::StorageBackendType sourceBackend, const mv::pointdata::storage::StorageBackendType targetBackend, const std::uint32_t targetType, const bool targetSafeValues)
{
    if constexpr (SourceType == 0)
        runConversionCase<float>(sourceBackend, targetBackend, SourceType, targetType, targetSafeValues);
    else if constexpr (SourceType == 1)
        runConversionCase<biovault::bfloat16_t>(sourceBackend, targetBackend, SourceType, targetType, targetSafeValues);
    else if constexpr (SourceType == 2)
        runConversionCase<std::int32_t>(sourceBackend, targetBackend, SourceType, targetType, targetSafeValues);
    else if constexpr (SourceType == 3)
        runConversionCase<std::uint32_t>(sourceBackend, targetBackend, SourceType, targetType, targetSafeValues);
    else if constexpr (SourceType == 4)
        runConversionCase<std::int16_t>(sourceBackend, targetBackend, SourceType, targetType, targetSafeValues);
    else if constexpr (SourceType == 5)
        runConversionCase<std::uint16_t>(sourceBackend, targetBackend, SourceType, targetType, targetSafeValues);
    else if constexpr (SourceType == 6)
        runConversionCase<std::int8_t>(sourceBackend, targetBackend, SourceType, targetType, targetSafeValues);
    else if constexpr (SourceType == 7)
        runConversionCase<std::uint8_t>(sourceBackend, targetBackend, SourceType, targetType, targetSafeValues);
}

GTEST_TEST(PointDataNumericConversion, matchesTheInAppElementTypeMatrix)
{
    constexpr std::array<const char*, numericTypeCount> typeNames{ "float", "bfloat16", "int32", "uint32", "int16", "uint16", "int8", "uint8" };
    constexpr std::array backendTypes{ mv::pointdata::storage::StorageBackendType::Dense, mv::pointdata::storage::StorageBackendType::Tiled, mv::pointdata::storage::StorageBackendType::Sparse };
    constexpr std::array<const char*, 3> backendNames{ "Dense", "Tiled", "Sparse" };

    for (std::size_t sourceBackendIndex = 0; sourceBackendIndex < backendTypes.size(); ++sourceBackendIndex)
    {
        for (std::size_t targetBackendIndex = 0; targetBackendIndex < backendTypes.size(); ++targetBackendIndex)
        {
            for (std::uint32_t sourceType = 0; sourceType < numericTypeCount; ++sourceType)
            {
                for (std::uint32_t targetType = 0; targetType < numericTypeCount; ++targetType)
                {
                    for (const bool targetSafeValues : { true, false })
                    {
                        SCOPED_TRACE(std::string(backendNames[sourceBackendIndex]) + " to " + backendNames[targetBackendIndex] + ": " + typeNames[sourceType] + " to " + typeNames[targetType] + (targetSafeValues ? " (target-safe values)" : " (range-boundary values)"));
                        switch (sourceType)
                        {
                            case 0: runSourceType<0>(backendTypes[sourceBackendIndex], backendTypes[targetBackendIndex], targetType, targetSafeValues); break;
                            case 1: runSourceType<1>(backendTypes[sourceBackendIndex], backendTypes[targetBackendIndex], targetType, targetSafeValues); break;
                            case 2: runSourceType<2>(backendTypes[sourceBackendIndex], backendTypes[targetBackendIndex], targetType, targetSafeValues); break;
                            case 3: runSourceType<3>(backendTypes[sourceBackendIndex], backendTypes[targetBackendIndex], targetType, targetSafeValues); break;
                            case 4: runSourceType<4>(backendTypes[sourceBackendIndex], backendTypes[targetBackendIndex], targetType, targetSafeValues); break;
                            case 5: runSourceType<5>(backendTypes[sourceBackendIndex], backendTypes[targetBackendIndex], targetType, targetSafeValues); break;
                            case 6: runSourceType<6>(backendTypes[sourceBackendIndex], backendTypes[targetBackendIndex], targetType, targetSafeValues); break;
                            case 7: runSourceType<7>(backendTypes[sourceBackendIndex], backendTypes[targetBackendIndex], targetType, targetSafeValues); break;
                            default: FAIL() << "Unsupported source type index: " << sourceType;
                        }
                    }
                }
            }
        }
    }
}
}
