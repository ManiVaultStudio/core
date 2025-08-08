// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "CheckableStringListModel.h"

#include "util/Exception.h"

#include <algorithm>

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
        return _checkStatesList[index.row()] ? Qt::Checked : Qt::Unchecked;

    return QStringListModel::data(index, role);
}

bool CheckableStringListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (role == Qt::CheckStateRole && index.isValid()) {
        _checkStatesList[index.row()] = value == Qt::Checked;

        emit dataChanged(index, index);

        return true;
    }

    return QStringListModel::setData(index, value, role);
}

void CheckableStringListModel::setStrings(const QStringList& strings)
{
    QStringListModel::setStringList(strings);

    _checkStatesList.resize(strings.count());

    _strings = strings;

    setCheckedIndicesFromStrings(_checkedStrings);
}

QStringList CheckableStringListModel::getCheckedStrings() const
{
    QStringList checkedStrings;

    checkedStrings.reserve(_checkStatesList.count());

    for (std::int32_t stringIndex = 0; stringIndex < _checkStatesList.count(); ++stringIndex)
        if (_checkStatesList[stringIndex])
            checkedStrings << data(index(stringIndex, 0), Qt::EditRole).toString();

    return checkedStrings;
}

CheckableStringListModel::CheckedIndicesList CheckableStringListModel::getCheckedIndicesList() const
{
    CheckedIndicesList checkedIndicesList;

    checkedIndicesList.reserve(_checkStatesList.count());

    for (std::int32_t stringIndex = 0; stringIndex < _checkStatesList.count(); ++stringIndex)
        if (_checkStatesList[stringIndex])
            checkedIndicesList << stringIndex;

    return checkedIndicesList;
}

CheckableStringListModel::CheckedIndicesSet CheckableStringListModel::getCheckedIndicesSet() const
{
    CheckedIndicesSet checkedIndicesSet;

    checkedIndicesSet.reserve(_checkStatesList.count());

    for (std::int32_t stringIndex = 0; stringIndex < _checkStatesList.count(); ++stringIndex)
        if (_checkStatesList[stringIndex])
            checkedIndicesSet << stringIndex;

    return checkedIndicesSet;
}

void CheckableStringListModel::setCheckedIndicesSet(const CheckedIndicesSet& checkedIndicesSet)
{
    const auto checkStatesList = _checkStatesList;

    std::fill(_checkStatesList.begin(), _checkStatesList.end(), false);

    for (const auto& checkedIndex : checkedIndicesSet)
        _checkStatesList[checkedIndex] = true;

    _checkedStrings = getCheckedStrings();

    if (checkStatesList != _checkStatesList)
        emit dataChanged(index(0, 0), index(rowCount() - 1));
}

void CheckableStringListModel::setCheckedIndicesFromStrings(const QStringList& checkedStrings)
{
    _checkedStrings = checkedStrings;

    const auto checkStatesList = _checkStatesList;

    std::fill(_checkStatesList.begin(), _checkStatesList.end(), false);

    for (const auto& checkedString : checkedStrings) {
        const auto index = _strings.indexOf(checkedString);

        if (index < 0)
            continue;

        _checkStatesList[index] = true;
    }

    if (checkStatesList != _checkStatesList)
        emit dataChanged(index(0, 0), index(rowCount() - 1));
}

void CheckableStringListModel::invertChecks()
{
    for (auto& checkState : _checkStatesList)
        checkState = !checkState;

    emit dataChanged(index(0, 0), index(rowCount() - 1));
}

}