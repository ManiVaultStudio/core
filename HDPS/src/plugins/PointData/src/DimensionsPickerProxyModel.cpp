// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 


// Its own header file:
#include "DimensionsPickerProxyModel.h"
#include "DimensionsPickerHolder.h"
#include "DimensionsPickerItemModel.h"

#include <QString>

// Standard C++ header files:
#include <limits>

namespace
{
}

namespace hdps
{

    DimensionsPickerProxyModel::DimensionsPickerProxyModel(const DimensionsPickerHolder& holder) :
        _holder(holder),
        _minimumStandardDeviation(-std::numeric_limits<double>::infinity())
    {

    }

    void DimensionsPickerProxyModel::SetMinimumStandardDeviation(const double minimumStandardDeviation)
    {
        _minimumStandardDeviation = minimumStandardDeviation;
    }

    bool DimensionsPickerProxyModel::lessThan(const QModelIndex &modelIndex1, const QModelIndex &modelIndex2) const
    {
        const auto column = modelIndex1.column();

        if ((column == modelIndex2.column()) && (column >= 0) && (column < static_cast<int>(DimensionsPickerItemModel::ColumnEnum::count)))
        {
            const auto row1 = modelIndex1.row();
            const auto row2 = modelIndex2.row();

            switch (static_cast<DimensionsPickerItemModel::ColumnEnum>(column))
            {
            case DimensionsPickerItemModel::ColumnEnum::Name:
            {
                return _holder.lessThanName(row1, row2);
            }
            case DimensionsPickerItemModel::ColumnEnum::Mean:
            {
                if (_holder._statistics.empty())
                {
                    break;
                }
                return _holder._statistics[row1].mean[_holder._ignoreZeroValues ? 1 : 0] <
                    _holder._statistics[row2].mean[_holder._ignoreZeroValues ? 1 : 0];
            }
            case DimensionsPickerItemModel::ColumnEnum::StandardDeviation:
            {
                if (_holder._statistics.empty())
                {
                    break;
                }
                return _holder._statistics[row1].standardDeviation[_holder._ignoreZeroValues ? 1 : 0] <
                    _holder._statistics[row2].standardDeviation[_holder._ignoreZeroValues ? 1 : 0];
            }
            }
        }
        return QSortFilterProxyModel::lessThan(modelIndex1, modelIndex2);
    }


    bool DimensionsPickerProxyModel::filterAcceptsRow(const int sourceRow, const QModelIndex &sourceParent) const
    {
        if ((_minimumStandardDeviation <= -std::numeric_limits<double>::infinity()) ||
            (_holder._statistics[sourceRow].standardDeviation[_holder._ignoreZeroValues ? 1 : 0] >= _minimumStandardDeviation))
        {
            // Note: When _minimumStandardDeviation is -infinity, we accept rows with standardDeviation = NaN.

            if (!_filterShouldAcceptOnlySelected || _holder.isDimensionEnabled(sourceRow))
            {
                const auto IsExcluded = [this, sourceRow]
                {
                    const auto name = _holder.getName(sourceRow);
                    const auto end = _exclusion.cend();
                    return std::find(_exclusion.cbegin(), end, name) != end;
                };

                if (!_filterShouldApplyExclusion || ! IsExcluded())
                {
                    // The default implementation returns true if the value held by the relevant item matches
                    // the filter string, wildcard string or regular expression
                    return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
                }
            }
        }
        return false;
    }

    void DimensionsPickerProxyModel::SetFilterShouldAcceptOnlySelected(const bool arg)
    {
        _filterShouldAcceptOnlySelected = arg;
    }

} // namespace hdps
