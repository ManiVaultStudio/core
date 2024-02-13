// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SelectionsModel.h"

#include <models/AbstractDataHierarchyModel.h>

#include <util/Exception.h>

#include <QDebug>

#ifdef _DEBUG
    #define SELECTIONS_MODEL_VERBOSE
#endif

using namespace mv;
using namespace mv::util;

SelectionsModel::Row::Row(Dataset<DatasetImpl> dataset) :
    QList<QStandardItem*>()
{
    append(new AbstractDataHierarchyModel::NameItem(dataset));
    append(new AbstractDataHierarchyModel::DatasetIdItem(dataset));
    append(new AbstractDataHierarchyModel::RawDataNameItem(dataset));
}

SelectionsModel::SelectionsModel(QObject* parent) :
    QStandardItemModel(parent),
    _countAction(this, "Number of selections")
{
    setHorizontalHeaderLabels({ "Name", "ID", "Raw data name" });

    _countAction.setToolTip("Number of selections");
    _countAction.setEnabled(false);

    connect(&mv::data(), &AbstractDataManager::selectionAdded, this, &SelectionsModel::populateFromDataManager);
    connect(&mv::data(), &AbstractDataManager::selectionRemoved, this, &SelectionsModel::populateFromDataManager);

    populateFromDataManager();
}

void SelectionsModel::populateFromDataManager()
{
#ifdef SELECTIONS_MODEL_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    setRowCount(0);

    for (const auto& dataset : mv::data().getAllDatasets())
        appendRow(Row(dataset));

    if (rowCount() == 1)
        _countAction.setString(QString("%1 selection").arg(rowCount()));
    else
        _countAction.setString(QString("%1 selections").arg(rowCount()));
}
