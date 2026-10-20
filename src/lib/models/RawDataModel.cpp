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

RawDataModel::Item::Item(const QString& rawDataName) :
    QStandardItem(),
    _rawDataName(rawDataName)
{
    Q_ASSERT(!_rawDataName.isEmpty());
}

QString RawDataModel::Item::getRawDataName() const
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
            return QVariant::fromValue(mv::data().getRawDataSize(getRawDataName()));

        case Qt::DisplayRole:
            return util::getNoBytesHumanReadable(data(Qt::EditRole).toLongLong());

        case Qt::ToolTipRole:
            return "Size: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return {};
}

RawDataModel::RawDataModel(QObject* parent) :
    StandardItemModel(parent),
    _overallSizeAction(this, "Overall size")
{
    setColumnCount(static_cast<int>(Column::Count));

    _overallSizeAction.setEnabled(false);

    connect(&mv::data(), &AbstractDataManager::rawDataAdded, this, &RawDataModel::populateFromDataManager);
    connect(&mv::data(), &AbstractDataManager::rawDataRemoved, this, &RawDataModel::populateFromDataManager);
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

    if (mv::projects().isOpeningProject() || mv::projects().isImportingProject())
        return;

    setRowCount(0);

    for (const auto& rawDataName : mv::data().getRawDataNames())
        appendRow(Row(rawDataName));

    _overallSizeAction.setString(QString("Overall size: %1").arg(util::getNoBytesHumanReadable(mv::data().getOverallRawDataSize())));
}

}