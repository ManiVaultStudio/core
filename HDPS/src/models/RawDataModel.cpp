// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "RawDataModel.h"

#include "models/AbstractDataHierarchyModel.h"

#include "util/Exception.h"

#include <QDebug>

#ifdef _DEBUG
    #define RAW_DATA_MODEL_VERBOSE
#endif

namespace mv {

using namespace util;

RawDataModel::RawDataItem::RawDataItem(const QString& rawDataName) :
    QStandardItem(),
    _rawDataName(rawDataName)
{
    Q_ASSERT(!_rawDataName.isEmpty());
}

QString RawDataModel::RawDataItem::getRawDataName() const
{
    return _rawDataName;
}

QVariant RawDataModel::RawDataNameItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getRawDataName();

        case Qt::ToolTipRole:
            return "Name: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return {};
}

QVariant RawDataModel::RawDataTypeItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return mv::data().getRawDataType(getRawDataName());

        case Qt::ToolTipRole:
            return "Type: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return {};
}

QVariant RawDataModel::RawDataSizeItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return mv::data().getRawDataSize(getRawDataName());

        case Qt::DisplayRole:
            return util::getNoBytesHumanReadable(data(Qt::DisplayRole).toLongLong());

        case Qt::ToolTipRole:
            return "Size: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return {};
}

RawDataModel::Row::Row(const QString& rawDataName) :
    QList<QStandardItem*>()
{
    append(new RawDataNameItem(rawDataName));
    //append(new RawDataTypeItem(rawDataName));
    //append(new RawDataSizeItem(rawDataName));
}

RawDataModel::RawDataModel(QObject* parent) :
    QStandardItemModel(parent),
    _countAction(this, "Number of raw data")
{
    setColumnCount(static_cast<int>(Column::Count));

    _countAction.setToolTip("Number of raw data items");
    _countAction.setEnabled(false);

    connect(&mv::data(), &AbstractDataManager::rawDataAdded, this, &RawDataModel::populateFromDataManager);
    connect(&mv::data(), &AbstractDataManager::rawDataRemoved, this, &RawDataModel::populateFromDataManager);

    populateFromDataManager();
}

QVariant RawDataModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    switch (static_cast<Column>(section))
    {
        case Column::Name:
            return RawDataNameItem::headerData(orientation, role);

        case Column::Type:
            return RawDataTypeItem::headerData(orientation, role);

        case Column::Size:
            return RawDataSizeItem::headerData(orientation, role);

        default:
            break;
    }

    return {};
}

void RawDataModel::populateFromDataManager()
{
#ifdef RAW_DATA_MODEL_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    setRowCount(0);

    //for (const auto& dataset : mv::data().getRawDataNames())
    //    appendRow(Row(dataset));

    if (rowCount() == 1)
        _countAction.setString(QString("%1 raw data").arg(rowCount()));
    else
        _countAction.setString(QString("%1 raw data").arg(rowCount()));
}

}