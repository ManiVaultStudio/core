// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DatasetsToRemoveModel.h"
#include "DataHierarchyItem.h"
#include "Set.h"

#include "util/Exception.h"

#include <QDebug>
#include <QIcon>

using namespace mv;
using namespace mv::util;

DatasetsToRemoveModel::Item::Item(Dataset<DatasetImpl> dataset, bool editable /*= false*/) :
    QStandardItem(),
    QObject(),
    _dataset(dataset)
{
    Q_ASSERT(_dataset.isValid());

    setEditable(editable);
}

QVariant DatasetsToRemoveModel::Item::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::ForegroundRole:
        {
            if (_dataset.isValid())
                break;

            return _dataset->isLocked() ? QApplication::palette().color(QPalette::Disabled, QPalette::Text).name() : QApplication::palette().color(QPalette::Normal, QPalette::Text).name();
        }

        default:
            break;
    }

    return QStandardItem::data(role);
}

Dataset<DatasetImpl>& DatasetsToRemoveModel::Item::getDataset()
{
    return _dataset;
}

const Dataset<DatasetImpl>& DatasetsToRemoveModel::Item::getDataset() const
{
    return _dataset;
}

DatasetsToRemoveModel::NameItem::NameItem(Dataset<DatasetImpl> dataset) :
    Item(dataset, true)
{
    setCheckable(true);
    setCheckState(Qt::Checked);

    connect(&getDataset(), &Dataset<DatasetImpl>::guiNameChanged, this, [this]() -> void {
        emitDataChanged();
    });
}

QVariant DatasetsToRemoveModel::NameItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getDataset().isValid() ? getDataset()->getGuiName() : "";

        case Qt::ToolTipRole:
            return QString("Dataset name: %1").arg(data(Qt::DisplayRole).toString());

        case Qt::DecorationRole:
        {
            if (!getDataset().isValid())
                break;

            return getDataset()->getIcon();
        }

        default:
            break;
    }

    return Item::data(role);
}

void DatasetsToRemoveModel::NameItem::setData(const QVariant& value, int role /* = Qt::UserRole + 1 */)
{
    switch (role) {
        case Qt::EditRole:
        {
            if (getDataset().isValid())
                getDataset()->setText(value.toString());

            break;
        }

        default:
            Item::setData(value, role);
    }
}

DatasetsToRemoveModel::DatasetIdItem::DatasetIdItem(Dataset<DatasetImpl> dataset) :
    Item(dataset)
{
    connect(getDataset().get(), &gui::WidgetAction::idChanged, this, [this](const QString& id) -> void {
        emitDataChanged();
    });
}

QVariant DatasetsToRemoveModel::DatasetIdItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getDataset().isValid() ? getDataset()->getId() : "";

        case Qt::ToolTipRole:
            return "Dataset identifier: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

DatasetsToRemoveModel::Row::Row(Dataset<DatasetImpl> dataset) :
    QList<QStandardItem*>()
{
    append(new NameItem(dataset));
    append(new DatasetIdItem(dataset));
}

DatasetsToRemoveModel::DatasetsToRemoveModel(QObject* parent) :
    QStandardItemModel(parent)
{
    setColumnCount(static_cast<int>(Column::Count));

    //for (const auto topLevelItem : dataHierarchy().getTopLevelItems())
    //    addDataHierarchyModelItem(*topLevelItem);
}

QVariant DatasetsToRemoveModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (static_cast<Column>(section))
    {
        case Column::Name:
            return NameItem::headerData(orientation, role);

        case Column::DatasetId:
            return DatasetIdItem::headerData(orientation, role);

        default:
            break;
    }

    return {};
}

void DatasetsToRemoveModel::setSelectedDatasets(mv::Datasets selectedDatasets)
{
    const auto printDatasets = [](QString message, Datasets datasets) -> void {
        qDebug() << "";
        qDebug() << message;

        for (auto dataset : datasets)
            qDebug() << dataset->getGuiName();
    };

    DataHierarchyItems selectedDataHierarchyItems;

    for (auto& selectedDataset : selectedDatasets)
        selectedDataHierarchyItems << &selectedDataset->getDataHierarchyItem();

    Datasets datasetsToRemove, topLevelDatasetsToRemove, descendantDatasetsToRemove;

    for (auto& topLevelDatasetCandidate : selectedDatasets)
        if (!topLevelDatasetCandidate->getDataHierarchyItem().isChildOf(selectedDataHierarchyItems))
            topLevelDatasetsToRemove << topLevelDatasetCandidate;

    printDatasets("Top-level datasets to remove", topLevelDatasetsToRemove);

    for (auto topLevelDatasetToRemove : topLevelDatasetsToRemove)
        for (auto descendantDataHierarchyItem : topLevelDatasetToRemove->getDataHierarchyItem().getChildren(true))
            descendantDatasetsToRemove << descendantDataHierarchyItem->getDataset();

    datasetsToRemove << topLevelDatasetsToRemove << descendantDatasetsToRemove;

    printDatasets("Datasets to remove", datasetsToRemove);

    // Sort based on item depth
    std::sort(datasetsToRemove.begin(), datasetsToRemove.end(), [](auto datasetA, auto datasetB) -> bool {
        return datasetA->getDataHierarchyItem().getDepth() < datasetB->getDataHierarchyItem().getDepth();
    });

    printDatasets("Depth-sorted datasets to remove", datasetsToRemove);

    // Remove duplicates
    datasetsToRemove.erase(std::unique(datasetsToRemove.begin(), datasetsToRemove.end()), datasetsToRemove.end());

    printDatasets("After removing duplicates", datasetsToRemove);

    for (auto topLevelDatasetToRemove : topLevelDatasetsToRemove)
        appendRow(Row(topLevelDatasetToRemove));

    for (auto descendantDatasetToRemove : descendantDatasetsToRemove) {
        const auto matches = match(index(0, static_cast<int>(Column::DatasetId), QModelIndex()), Qt::EditRole, descendantDatasetToRemove->getDataHierarchyItem().getParent()->getDataset()->getId(), -1, Qt::MatchFlag::MatchExactly | Qt::MatchFlag::MatchRecursive);

        if (matches.isEmpty())
            throw std::runtime_error("Parent data hierarchy item not found in model");

        itemFromIndex(matches.first().siblingAtColumn(static_cast<int>(Column::Name)))->appendRow(Row(descendantDatasetToRemove));
    }

    // Bottom-up
    std::reverse(datasetsToRemove.begin(), datasetsToRemove.end());

    printDatasets("After reversing", datasetsToRemove);
}

void DatasetsToRemoveModel::addDataset(Dataset<DatasetImpl> dataset, mv::Datasets selectedDatasets)
{
    try {

#ifdef _DEBUG
        qDebug() << "Add dataset" << dataset->getGuiName() << "to the data datasets to remove model";
#endif

        if (!match(index(0, static_cast<int>(Column::DatasetId), QModelIndex()), Qt::EditRole, dataset->getId(), -1, Qt::MatchFlag::MatchExactly | Qt::MatchFlag::MatchRecursive).isEmpty())
            return;

        if (selectedDatasets.contains(dataset) && dataset->getDataHierarchyItem().hasParent()) {
            const auto matches = match(index(0, static_cast<int>(Column::DatasetId), QModelIndex()), Qt::EditRole, dataset->getDataHierarchyItem().getParent()->getDataset()->getId(), -1, Qt::MatchFlag::MatchExactly | Qt::MatchFlag::MatchRecursive);

            if (matches.isEmpty())
                throw std::runtime_error("Parent data hierarchy item not found in model");

            itemFromIndex(matches.first().siblingAtColumn(static_cast<int>(Column::Name)))->appendRow(Row(dataset));
        }
        else {
            appendRow(Row(dataset));
        }
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to add item to the datasets to remove model", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to add item to the datasets to remove model");
    }
}
