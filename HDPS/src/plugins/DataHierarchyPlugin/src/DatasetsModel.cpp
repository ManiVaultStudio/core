// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DatasetsModel.h"

#include <models/AbstractDataHierarchyModel.h>

#include <util/Exception.h>

#include <QDebug>

#ifdef _DEBUG
    #define DATASETS_MODEL_VERBOSE
#endif

using namespace mv;
using namespace mv::util;

DatasetsModel::Row::Row(Dataset<DatasetImpl> dataset) :
    QList<QStandardItem*>()
{
    append(new AbstractDataHierarchyModel::NameItem(dataset));
    append(new AbstractDataHierarchyModel::DatasetIdItem(dataset));
    append(new AbstractDataHierarchyModel::RawDataNameItem(dataset));
    append(new AbstractDataHierarchyModel::SourceDatasetIdItem(dataset));
}

DatasetsModel::DatasetsModel(QObject* parent) :
    QStandardItemModel(parent),
    _countAction(this, "Number of datasets")
{
    setColumnCount(static_cast<int>(Column::Count));

    _countAction.setToolTip("Number of datasets");
    _countAction.setEnabled(false);

    connect(&mv::data(), &AbstractDataManager::datasetAdded, this, &DatasetsModel::populateFromDataManager);
    connect(&mv::data(), &AbstractDataManager::datasetRemoved, this, &DatasetsModel::populateFromDataManager);

    populateFromDataManager();
}

QVariant DatasetsModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    switch (static_cast<Column>(section))
    {
        case Column::Name:
            return AbstractDataHierarchyModel::NameItem::headerData(orientation, role);

        case Column::DatasetId:
            return AbstractDataHierarchyModel::DatasetIdItem::headerData(orientation, role);

        case Column::RawDataName:
            return AbstractDataHierarchyModel::RawDataNameItem::headerData(orientation, role);

        case Column::SourceDatasetId:
            return AbstractDataHierarchyModel::SourceDatasetIdItem::headerData(orientation, role);

        default:
            break;
    }

    return {};
}

void DatasetsModel::populateFromDataManager()
{
#ifdef DATASETS_MODEL_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    setRowCount(0);

    for (const auto& dataset : mv::data().getAllDatasets())
        appendRow(Row(dataset));

    if (rowCount() == 1)
        _countAction.setString(QString("%1 dataset").arg(rowCount()));
    else
        _countAction.setString(QString("%1 datasets").arg(rowCount()));
}
