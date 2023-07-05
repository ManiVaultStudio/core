// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once
#include <algorithm> // For fill_n.
#include <cassert>
#include <memory> // For unique_ptr.
#include <vector>

#include <QString>

namespace hdps
{
    struct StatisticsPerDimension
    {
        // mean[0] and standardDeviation[0] include zero-valued data.
        // mean[1] and standardDeviation[1] ignore zero-valued data.
        double mean[2];
        double standardDeviation[2];
    };

    class DimensionsPickerHolder
    {
        std::unique_ptr<QString[]> _names;
        std::unique_ptr<bool[]> _enabledDimensions;
        unsigned _numberOfDimensions{};

    public:
        std::vector<StatisticsPerDimension> _statistics;
        std::vector<double> distinctStandardDeviationsWithAndWithoutZero[2];
        bool _ignoreZeroValues{};
        
        // C++ "Rule of Zero": implicitly defaulted move-constructor and move assignment are just fine.

        DimensionsPickerHolder() = default;

        explicit DimensionsPickerHolder(const unsigned numberOfDimensions);

        explicit DimensionsPickerHolder(
            const QString* const names,
            const unsigned numberOfDimensions);

        unsigned getNumberOfDimensions() const noexcept;

        unsigned getNumberOfSelectedDimensions() const noexcept;

        QString getName(const std::size_t) const;

        bool lessThanName(std::size_t, std::size_t) const noexcept;

        void disableAllDimensions();

        bool tryToEnableDimensionByName(const QString& name);

        bool isDimensionEnabled(std::size_t) const;

        void toggleDimensionEnabled(std::size_t);

        void setDimensionEnabled(std::size_t, bool);

        std::vector<bool> getEnabledDimensions() const;
    };

} // namespace hdps
