// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once
#include <QSortFilterProxyModel>

namespace hdps
{
    class DimensionsPickerHolder;

    class DimensionsPickerProxyModel final : public QSortFilterProxyModel
    {
        const DimensionsPickerHolder& _holder;
        double _minimumStandardDeviation;
        bool _filterShouldAcceptOnlySelected = false;
        bool _filterShouldApplyExclusion = false;
        std::vector<QString> _exclusion;

    public:
        explicit DimensionsPickerProxyModel(const DimensionsPickerHolder& holder);

        void SetMinimumStandardDeviation(const double minimumStandardDeviation);

        void SetFilterShouldAcceptOnlySelected(bool);

        void SetFilterShouldAppyExclusion(const bool arg)
        {
            _filterShouldApplyExclusion = arg;
        }

        void SetExclusion(std::vector<QString> arg)
        {
            _exclusion = std::move(arg);
        }

    private:
        bool lessThan(const QModelIndex &modelIndex1, const QModelIndex &modelIndex2) const override;

    public:
        bool filterAcceptsRow(const int sourceRow, const QModelIndex &sourceParent) const override;
    };

}

