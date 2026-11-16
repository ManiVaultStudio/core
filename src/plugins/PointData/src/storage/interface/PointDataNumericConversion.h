// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2026 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include <biovault_bfloat16/biovault_bfloat16.h>

#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <type_traits>

namespace mv::pointdata::storage::detail
{
template <typename TargetType, typename SourceType>
void convertNumericRowTo(const char* source, const std::uint64_t columns, std::uint8_t* destination)
{
    for (std::uint64_t column = 0; column < columns; ++column)
    {
        SourceType sourceValue;
        std::memcpy(&sourceValue, source + column * sizeof(SourceType), sizeof(SourceType));
        const auto value = [&] {
            if constexpr (std::is_same_v<SourceType, biovault::bfloat16_t>)
                return static_cast<long double>(static_cast<float>(sourceValue));
            else
                return static_cast<long double>(sourceValue);
        }();

        TargetType targetValue;
        if constexpr (std::is_integral_v<TargetType>)
        {
            if (!std::isfinite(value) || value < static_cast<long double>(std::numeric_limits<TargetType>::lowest()) || value > static_cast<long double>(std::numeric_limits<TargetType>::max()))
                throw std::range_error("A value cannot be represented by the selected integer element type");
            targetValue = static_cast<TargetType>(value);
        }
        else if constexpr (std::is_same_v<TargetType, biovault::bfloat16_t>)
            targetValue = biovault::bfloat16_t(static_cast<float>(value));
        else
            targetValue = static_cast<TargetType>(value);

        std::memcpy(destination + column * sizeof(TargetType), &targetValue, sizeof(TargetType));
    }
}

template <typename SourceType>
void convertNumericRowToType(const std::uint32_t targetTypeIndex, const char* source, const std::uint64_t columns, std::uint8_t* destination)
{
    switch (targetTypeIndex)
    {
        case 0: return convertNumericRowTo<float, SourceType>(source, columns, destination);
        case 1: return convertNumericRowTo<biovault::bfloat16_t, SourceType>(source, columns, destination);
        case 2: return convertNumericRowTo<std::int32_t, SourceType>(source, columns, destination);
        case 3: return convertNumericRowTo<std::uint32_t, SourceType>(source, columns, destination);
        case 4: return convertNumericRowTo<std::int16_t, SourceType>(source, columns, destination);
        case 5: return convertNumericRowTo<std::uint16_t, SourceType>(source, columns, destination);
        case 6: return convertNumericRowTo<std::int8_t, SourceType>(source, columns, destination);
        case 7: return convertNumericRowTo<std::uint8_t, SourceType>(source, columns, destination);
        default: throw std::invalid_argument("Point-data conversion uses an unsupported target element type");
    }
}

inline void convertNumericRow(const std::uint32_t sourceTypeIndex, const std::uint32_t targetTypeIndex, const char* source, const std::uint64_t columns, std::uint8_t* destination)
{
    if (columns != 0 && (!source || !destination))
        throw std::invalid_argument("Point-data conversion row is unavailable");

    switch (sourceTypeIndex)
    {
        case 0: return convertNumericRowToType<float>(targetTypeIndex, source, columns, destination);
        case 1: return convertNumericRowToType<biovault::bfloat16_t>(targetTypeIndex, source, columns, destination);
        case 2: return convertNumericRowToType<std::int32_t>(targetTypeIndex, source, columns, destination);
        case 3: return convertNumericRowToType<std::uint32_t>(targetTypeIndex, source, columns, destination);
        case 4: return convertNumericRowToType<std::int16_t>(targetTypeIndex, source, columns, destination);
        case 5: return convertNumericRowToType<std::uint16_t>(targetTypeIndex, source, columns, destination);
        case 6: return convertNumericRowToType<std::int8_t>(targetTypeIndex, source, columns, destination);
        case 7: return convertNumericRowToType<std::uint8_t>(targetTypeIndex, source, columns, destination);
        default: throw std::invalid_argument("Point-data conversion uses an unsupported source element type");
    }
}
}
