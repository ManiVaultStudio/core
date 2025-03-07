// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 


// Its own header file:
#include "DimensionsPickerItemModel.h"
#include "DimensionsPickerHolder.h"

// Qt header files:
#include <QString>

// Standard C++ header files:
#include <cmath> // For isnan
#include <vector>

namespace
{
    QVariant ToVariant(const double value)
    {
        return std::isnan(value) ? QVariant{} : QString::number(value, 'f', 2);
    }

}

namespace mv
{

    DimensionsPickerItemModel::DimensionsPickerItemModel(
        DimensionsPickerHolder& holder)
        :
        _holder(holder)
    {
    }


    int DimensionsPickerItemModel::rowCount(const QModelIndex &) const
    {
        const auto rowCount = _holder.getNumberOfDimensions();
        assert(rowCount <= INT_MAX);
        return static_cast<int>(rowCount);
    }


    int DimensionsPickerItemModel::columnCount(const QModelIndex &) const noexcept
    {
        return static_cast<int>(ColumnEnum::count);
    }


    QModelIndex DimensionsPickerItemModel::index(const int row, const int column, const QModelIndex &) const
    {
        return createIndex(row, column);

    }


    QModelIndex DimensionsPickerItemModel::parent(const QModelIndex &) const noexcept
    {
        return{};
    }


    QVariant DimensionsPickerItemModel::data(const QModelIndex & modelIndex, const int role) const
    {
        try
        {
            if (modelIndex.isValid())
            {
                const auto row = static_cast<unsigned>(modelIndex.row());
                assert(row < _holder.getNumberOfDimensions());
                const auto column = static_cast<unsigned>(modelIndex.column());

                if ((role == Qt::DisplayRole) && (column < static_cast<unsigned>(ColumnEnum::count)))
                {
                    switch (static_cast<ColumnEnum>(column))
                    {
                    case ColumnEnum::Name:
                        return _holder.getName(row);
                    case ColumnEnum::Mean:
                        return _holder._statistics.empty() ? QVariant{} : ToVariant(_holder._statistics[row].mean[_holder._ignoreZeroValues ? 1 : 0]);
                    case ColumnEnum::StandardDeviation:
                        return _holder._statistics.empty() ? QVariant{} : ToVariant(_holder._statistics[row].standardDeviation[_holder._ignoreZeroValues ? 1 : 0]);
                    }
                }
                if ((role == Qt::CheckStateRole) && (column == 0))
                {
                    return _holder.isDimensionEnabled(row) ? Qt::Checked : Qt::Unchecked;
                }
            }
        }
        catch (const std::exception&)
        {
            // Out of luck.
        }
        return {};
    }


    Qt::ItemFlags DimensionsPickerItemModel::flags(const QModelIndex& modelIndex) const noexcept
    {
        // https://doc.qt.io/qt-5/qabstractitemmodel.html#flags
        // The base class implementation returns a combination of flags that enables the
        // item (ItemIsEnabled) and allows it to be selected ().
        const auto flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

        if (modelIndex.isValid() && (modelIndex.column() == 0))
        {
            return flags | Qt::ItemIsUserCheckable;
        }
        else
        {
            return flags;
        }
    }


    bool DimensionsPickerItemModel::setData(const QModelIndex &modelIndex, const QVariant &value, int role)
    {
        if ((role == Qt::CheckStateRole) && modelIndex.isValid() && (modelIndex.column() == 0))
        {
            _holder.toggleDimensionEnabled(static_cast<unsigned>(modelIndex.row()));

            // https://doc.qt.io/qt-5/qabstractitemmodel.html#setData
            // "The dataChanged() signal should be emitted if the data was successfully set."
            Q_EMIT dataChanged(modelIndex, modelIndex, QVector<int>(1, Qt::CheckStateRole));


            return true;
        }
        else
        {
            // https://doc.qt.io/qt-5/qabstractitemmodel.html#setData
            // "The base class implementation returns false."
            return false;
        }
        return QAbstractItemModel::setData(modelIndex, value, role);
    }

    QVariant DimensionsPickerItemModel::headerData(const int section, const Qt::Orientation orientation, const int role) const
    {
        try
        {
            if (orientation == Qt::Horizontal && role == Qt::DisplayRole && (section >= 0) && (section < static_cast<unsigned>(ColumnEnum::count) ))
            {
                switch (static_cast<ColumnEnum>(section))
                {
                case ColumnEnum::Name: return tr("Name");
                case ColumnEnum::Mean: return tr("M");
                case ColumnEnum::StandardDeviation: return tr("SD");
                }
            }
        }
        catch (const std::exception&)
        {
            assert(!"Exceptions are not allowed here!");
        }
        return QAbstractItemModel::headerData(section, orientation, role);

    }

} // namespace mv
