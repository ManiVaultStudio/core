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
        return _checkedItems[index.row()] ? Qt::Checked : Qt::Unchecked;

    return QStringListModel::data(index, role);
}

bool CheckableStringListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (role == Qt::CheckStateRole && index.isValid()) {
        if (value == Qt::Checked)
            _checkedItems[index.row()] = true;
        else
            _checkedItems[index.row()] = false;

        emit dataChanged(index, index);

        return true;
    }

    return QStringListModel::setData(index, value, role);
}

void CheckableStringListModel::setStrings(const QStringList& strings)
{
    QStringListModel::setStringList(strings);

    _checkedItems.resize(strings.count());

    std::fill(_checkedItems.begin(), _checkedItems.end(), false);
}

QStringList CheckableStringListModel::getCheckedStrings() const
{
    QStringList checkedStrings;

    checkedStrings.reserve(_checkedItems.count());

    for (int itemIndex = 0; itemIndex < _checkedItems.count(); ++itemIndex)
        if (_checkedItems[itemIndex])
            checkedStrings.push_back(data(index(itemIndex, 0), Qt::EditRole).toString());

    return checkedStrings;
}

CheckableStringListModel::StringIndicesSet CheckableStringListModel::getCheckedIndicesSet() const
{
    const auto checkedIndicesList = getCheckedIndicesList();

    return StringIndicesSet(checkedIndicesList.begin(), checkedIndicesList.end());
}

mv::CheckableStringListModel::StringIndicesList CheckableStringListModel::getCheckedIndicesList() const
{
    StringIndicesList checkedIndicesList;

    for (int itemIndex = 0; itemIndex < _checkedItems.count(); ++itemIndex)
        if (_checkedItems[itemIndex])
            checkedIndicesList << itemIndex;

    return checkedIndicesList;
}

QVariant CheckableStringListModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    if (section != 0)
        return {};

    if (orientation != Qt::Horizontal)
        return {};

    switch (role) {
        case Qt::DisplayRole:
            return "String";

        case Qt::ToolTipRole:
            return "String value";

        default:
            return {};
    }

    return {};
}

}