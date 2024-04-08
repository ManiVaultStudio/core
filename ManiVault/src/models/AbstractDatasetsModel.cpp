// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AbstractDatasetsModel.h"

#include "Application.h"
#include "CoreInterface.h"
#include "Set.h"

#include <util/Exception.h>

#include <actions/WidgetAction.h>

#ifdef _DEBUG
    //#define ABSTRACT_DATASETS_MODEL_VERBOSE
#endif

namespace mv
{

using namespace util;
using namespace gui;

AbstractDatasetsModel::HeaderItem::HeaderItem(const ColumHeaderInfo& columHeaderInfo) :
    QStandardItem(),
    _columHeaderInfo(columHeaderInfo)
{
}

QVariant AbstractDatasetsModel::HeaderItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role)
    {
        case Qt::DisplayRole:
            return _columHeaderInfo._display;

        case Qt::EditRole:
            return _columHeaderInfo._edit;

        case Qt::ToolTipRole:
            return _columHeaderInfo._tooltip;

        default:
            break;
    }

    return QVariant();
}

AbstractDatasetsModel::Item::Item(Dataset<DatasetImpl> dataset, bool editable /*= false*/) :
    QStandardItem(),
    QObject(),
    _dataset(dataset)
{
    Q_ASSERT(_dataset.isValid());

    setEditable(editable);
    setDropEnabled(true);
}

Dataset<DatasetImpl>& AbstractDatasetsModel::Item::getDataset()
{
    return _dataset;
}

AbstractDatasetsModel::NameItem::NameItem(Dataset<DatasetImpl> dataset) :
    Item(dataset)
{
    //connect(&getDataset(), &Dataset<DatasetImpl>::textChanged, this, [this](const QString& name) -> void {
    //    emitDataChanged();
    //});
}

QVariant AbstractDatasetsModel::NameItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return const_cast<AbstractDatasetsModel::NameItem*>(this)->getDataset()->getGuiName();

        case Qt::ToolTipRole:
            return "Dataset name: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

AbstractDatasetsModel::LocationItem::LocationItem(Dataset<DatasetImpl> dataset) :
    Item(dataset)
{
    //connect(&getDataset(), &WidgetAction::textChanged, this, [this](const QString& name) -> void {
    //    emitDataChanged();
    //});
}

QVariant AbstractDatasetsModel::LocationItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return const_cast<AbstractDatasetsModel::LocationItem*>(this)->getDataset()->getLocation();

        case Qt::ToolTipRole:
            return "Dataset location: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}


QVariant AbstractDatasetsModel::IdItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            const_cast<AbstractDatasetsModel::IdItem*>(this)->getDataset()->getId();

        case Qt::ToolTipRole:
            return "Dataset globally unique identifier: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QMap<AbstractDatasetsModel::Column, AbstractDatasetsModel::ColumHeaderInfo> AbstractDatasetsModel::columnInfo = QMap<AbstractDatasetsModel::Column, AbstractDatasetsModel::ColumHeaderInfo>({
    { AbstractDatasetsModel::Column::Name, { "Name" , "Name", "Name of the dataset" } },
    { AbstractDatasetsModel::Column::Location, { "" , "Enabled", "Whether the dataset is enabled or not" } },
    { AbstractDatasetsModel::Column::ID, { "ID",  "ID", "Globally unique identifier of the dataset" } }
});

AbstractDatasetsModel::AbstractDatasetsModel(QObject* parent /*= nullptr*/) :
    QStandardItemModel(parent)
{
    setColumnCount(static_cast<int>(Column::Count));

    for (auto column : columnInfo.keys())
        setHorizontalHeaderItem(static_cast<int>(column), new HeaderItem(columnInfo[column]));

    connect(&mv::data(), &AbstractDataManager::datasetAdded, this, [this](Dataset<DatasetImpl> dataset, Dataset<DatasetImpl> parentDataset, bool visible = true) -> void {
        addDataset(dataset);
    });

    connect(&mv::data(), &AbstractDataManager::datasetAboutToBeRemoved, this, &AbstractDatasetsModel::removeDataset);
}

QStandardItem* AbstractDatasetsModel::itemFromDataset(Dataset<DatasetImpl> dataset) const
{
    const auto matches = match(index(0, static_cast<int>(Column::ID)), Qt::EditRole, dataset->getId(), 1, Qt::MatchExactly | Qt::MatchRecursive);

    if (matches.isEmpty())
        throw std::runtime_error(QString("%1 (%2) not found").arg(dataset->getGuiName(), dataset->getId()).toStdString());

    return itemFromIndex(matches.first().siblingAtColumn(static_cast<int>(Column::Name)));
}

void AbstractDatasetsModel::removeDataset(Dataset<DatasetImpl> dataset)
{
    try {
        auto datasetItem = itemFromDataset(dataset);

#ifdef ABSTRACT_DATASETS_MODEL_VERBOSE
        qDebug() << "AbstractDatasetsModel: Remove dataset:" << dataset->getGuiName();
#endif

        if (!removeRow(datasetItem->row()))
            throw std::runtime_error("Remove row failed");
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to remove dataset from datasets model", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to remove dataset from datasets model");
    }
}

}
