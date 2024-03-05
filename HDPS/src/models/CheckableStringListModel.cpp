// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "CheckableStringListModel.h"

#include "util/Exception.h"

#include <QDebug>

#ifdef _DEBUG
    #define CHECKABLE_STRING_LIST_MODEL_VERBOSE
#endif

namespace mv {

Qt::ItemFlags CheckableStringListModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QStringListModel::flags(index);

    if (index.isValid())
        flags |= Qt::ItemIsUserCheckable;

    return flags;
}

QVariant CheckableStringListModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::CheckStateRole && index.isValid())
        return (_checkedItems.contains(index.row())) ? Qt::Checked : Qt::Unchecked;

    return QStringListModel::data(index, role);
}

bool CheckableStringListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (role == Qt::CheckStateRole && index.isValid()) {
        if (value == Qt::Checked)
            _checkedItems.insert(index.row());
        else
            _checkedItems.remove(index.row());

        emit dataChanged(index, index);

        return true;
    }

    return QStringListModel::setData(index, value, role);
}

QStringList CheckableStringListModel::getCheckedStrings() const
{
    QStringList checkedStrings;

    checkedStrings.reserve(_checkedItems.count());

    for (const auto& checkedItem : _checkedItems)
        checkedStrings.push_back(data(index(checkedItem, 0), Qt::EditRole).toString());

    return checkedStrings;
}

CheckableStringListModel::StringIndicesSet CheckableStringListModel::getCheckedIndicesSet() const
{
    return _checkedItems;
}

mv::CheckableStringListModel::StringIndicesList CheckableStringListModel::getCheckedIndicesList() const
{
    return StringIndicesList(_checkedItems.begin(), _checkedItems.end());
}

}