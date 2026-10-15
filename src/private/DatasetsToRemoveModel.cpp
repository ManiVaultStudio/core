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
using namespace mv::gui;

DatasetsToRemoveModel::Item::Item(Dataset<DatasetImpl> dataset, DatasetsToRemoveModel& datasetsToRemoveModel, bool editable /*= false*/) :
    QStandardItem(),
    QObject(),
    _datasetsToRemoveModel(datasetsToRemoveModel),
    _dataset(dataset)
{
    Q_ASSERT(_dataset.isValid());

    if (!_dataset->isVisible())
        return;

    setEditable(editable);
}

DatasetsToRemoveModel::Item::~Item()
{
    if (column() > 0)
        return;

    const auto it = std::find_if(Row::allRows.begin(), Row::allRows.end(), [this](auto& row) -> bool {
        return row.first() == this;
    });

    Row::allRows.erase(it);
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

DatasetsToRemoveModel& DatasetsToRemoveModel::Item::getDatasetRemoveModel()
{
    return _datasetsToRemoveModel;
}

void DatasetsToRemoveModel::Item::updateReadOnly()
{
    if (!index().isValid())
        return;

    auto nameItem = model()->itemFromIndex(index().siblingAtColumn(static_cast<int>(Column::Name)));

    const auto hasParent = nameItem->parent() != nullptr;

    if (settings().getMiscellaneousSettings().getKeepDescendantsAfterRemovalAction().isChecked()) {
        if (getDataset()->mayUnderive())
            setEnabled(true);
        else
            setEnabled(hasParent ? nameItem->parent()->checkState() == Qt::Unchecked : true);
    }
    else {
        setEnabled(!hasParent);
    }

    emitDataChanged();
}

void DatasetsToRemoveModel::Item::initialize()
{
    updateReadOnly();

    connect(&settings().getMiscellaneousSettings().getKeepDescendantsAfterRemovalAction(), &ToggleAction::toggled, this, &Item::updateReadOnly);
}

DatasetsToRemoveModel::NameItem::NameItem(Dataset<DatasetImpl> dataset, DatasetsToRemoveModel& datasetsToRemoveModel) :
    Item(dataset, datasetsToRemoveModel, true)
{
    setCheckable(true);

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

            return StyledIcon(getDataset()->icon());
        }

        case Qt::CheckStateRole:
        {
            break;
        }

        default:
            break;
    }

    return Item::data(role);
}

void DatasetsToRemoveModel::NameItem::setKeepDescendants(bool keepDescendants)
{
    const auto keepDescendantsCheckState = keepDescendants ? Qt::Checked : Qt::Unchecked;

    auto parentItem = QStandardItem::parent();

    if (getDataset()->mayUnderive())
        setCheckState(parentItem == nullptr ? Qt::Checked : (keepDescendants ? Qt::Unchecked : Qt::Checked));
    else {
        if (parentItem != nullptr)
            setCheckState(parentItem->checkState() == Qt::Unchecked ? keepDescendantsCheckState : Qt::Checked);
        else
            setCheckState(keepDescendantsCheckState ? Qt::Checked : Qt::Unchecked);
    }
}

void DatasetsToRemoveModel::NameItem::updateCheckState()
{
    if (getDataset()->mayUnderive())
        return;

    auto parentItem = QStandardItem::parent();

    if (parentItem != nullptr && parentItem->checkState() == Qt::Checked)
        setCheckState(Qt::Checked);
}

DatasetsToRemoveModel::DatasetIdItem::DatasetIdItem(Dataset<DatasetImpl> dataset, DatasetsToRemoveModel& datasetsToRemoveModel) :
    Item(dataset, datasetsToRemoveModel)
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

DatasetsToRemoveModel::InfoItem::InfoItem(mv::Dataset<mv::DatasetImpl> dataset, DatasetsToRemoveModel& datasetsToRemoveModel) :
    Item(dataset, datasetsToRemoveModel)
{
}

QVariant DatasetsToRemoveModel::InfoItem::data(int role /*= Qt::UserRole + 1*/) const
{
    auto parentItem = QStandardItem::parent();

    const auto showInfo = !getDataset()->mayUnderive() && parentItem && parentItem->checkState() == Qt::Checked;

    switch (role) {
        case Qt::DecorationRole:
        {
            if (showInfo)
                return StyledIcon("info-circle");

            break;
        }

        case Qt::ToolTipRole:
        {
            if (showInfo)
                return "Dataset must co-exists with its parent dataset and therefore must also be removed";

            break;
        }

        default:
            break;
    }

    return Item::data(role);
}

std::vector<DatasetsToRemoveModel::Row> DatasetsToRemoveModel::Row::allRows = std::vector<DatasetsToRemoveModel::Row>();

DatasetsToRemoveModel::Row::Row(Dataset<DatasetImpl> dataset, DatasetsToRemoveModel& datasetsToRemoveModel) :
    QList<QStandardItem*>()
{
    append(new NameItem(dataset, datasetsToRemoveModel));
    append(new DatasetIdItem(dataset, datasetsToRemoveModel));
    append(new InfoItem(dataset, datasetsToRemoveModel));

    Row::allRows.push_back(*this);
}

DatasetsToRemoveModel::NameItem* DatasetsToRemoveModel::Row::getNameItem()
{
    return static_cast<NameItem*>(first());
}

void DatasetsToRemoveModel::Row::updateReadOnly()
{
    for (auto item : *this)
        static_cast<Item*>(item)->updateReadOnly();
}

void DatasetsToRemoveModel::Row::setKeepDescendants(bool keepDescendants)
{
    getNameItem()->setKeepDescendants(keepDescendants);
}

DatasetsToRemoveModel::DatasetsToRemoveModel(QObject* parent) :
    QStandardItemModel(parent)
{
    setColumnCount(static_cast<int>(Column::Count));

    connect(&settings().getMiscellaneousSettings().getKeepDescendantsAfterRemovalAction(), &ToggleAction::toggled, this, &DatasetsToRemoveModel::updateReadOnlyForAllNameItems);
    connect(&settings().getMiscellaneousSettings().getKeepDescendantsAfterRemovalAction(), &ToggleAction::toggled, this, &DatasetsToRemoveModel::setKeepDescendantsForAllNameItems);

    connect(this, &QStandardItemModel::dataChanged, this, [this](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles) -> void {
        if (roles.isEmpty())
            return;

        if (roles.first() == Qt::CheckStateRole) {
            updateReadOnlyForAllNameItems();
            updateCheckStatesForAllNameItems();
        }
    });
}

QVariant DatasetsToRemoveModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (static_cast<Column>(section))
    {
        case Column::Name:
            return NameItem::headerData(orientation, role);

        case Column::DatasetId:
            return DatasetIdItem::headerData(orientation, role);

        case Column::Info:
            return InfoItem::headerData(orientation, role);

        default:
            break;
    }

    return {};
}

void DatasetsToRemoveModel::setDatasets(mv::Datasets datasets)
{
    Row::allRows.clear();

    DataHierarchyItems selectedDataHierarchyItems, topLevelSelectedDataHierarchyItems;

    for (auto& selectedDataset : datasets)
        selectedDataHierarchyItems << &selectedDataset->getDataHierarchyItem();

    Datasets datasetsToRemove, topLevelDatasetsToRemove, descendantDatasetsToRemove;

    for (auto& topLevelDatasetCandidate : datasets)
        if (!topLevelDatasetCandidate->getDataHierarchyItem().isChildOf(selectedDataHierarchyItems))
            topLevelDatasetsToRemove << topLevelDatasetCandidate;

    for (auto topLevelDatasetToRemove : topLevelDatasetsToRemove)
        for (auto descendantDataHierarchyItem : topLevelDatasetToRemove->getDataHierarchyItem().getChildren(true))
            descendantDatasetsToRemove << descendantDataHierarchyItem->getDataset();

    for (auto topLevelDatasetToRemove : topLevelDatasetsToRemove)
        topLevelSelectedDataHierarchyItems << &topLevelDatasetToRemove->getDataHierarchyItem();

    datasetsToRemove << topLevelDatasetsToRemove << descendantDatasetsToRemove;

    std::sort(datasetsToRemove.begin(), datasetsToRemove.end(), [](auto datasetA, auto datasetB) -> bool {
        return datasetA->getDataHierarchyItem().getDepth() < datasetB->getDataHierarchyItem().getDepth();
    });

    datasetsToRemove.erase(std::unique(datasetsToRemove.begin(), datasetsToRemove.end()), datasetsToRemove.end());

    for (auto topLevelDatasetToRemove : topLevelDatasetsToRemove)
        appendRow(Row(topLevelDatasetToRemove, *this));

    for (auto descendantDatasetToRemove : descendantDatasetsToRemove) {
        const auto matches = match(index(0, static_cast<int>(Column::DatasetId), QModelIndex()), Qt::EditRole, descendantDatasetToRemove->getDataHierarchyItem().getParent()->getDataset()->getId(), -1, Qt::MatchFlag::MatchExactly | Qt::MatchFlag::MatchRecursive);

        if (matches.isEmpty())
            throw std::runtime_error("Parent data hierarchy item not found in model");

        itemFromIndex(matches.first().siblingAtColumn(static_cast<int>(Column::Name)))->appendRow(Row(descendantDatasetToRemove, *this));
    }

    std::reverse(datasetsToRemove.begin(), datasetsToRemove.end());

    setKeepDescendantsForAllNameItems();
    updateReadOnlyForAllNameItems();
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
                throw std::runtime_error("Parent data hierarchy item index not found in model");

            auto nameItem = itemFromIndex(matches.first().siblingAtColumn(static_cast<int>(Column::Name)));

            if (nameItem == nullptr)
                throw std::runtime_error("Parent data hierarchy item not found in model");

            nameItem->appendRow(Row(dataset, *this));
        }
        else {
            appendRow(Row(dataset, *this));
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

mv::Datasets DatasetsToRemoveModel::getDatasetsToRemove() const
{
    mv::Datasets datasetsToRemove;

    const auto numberOfChildren = rowCount();

    for (auto& row : Row::allRows)
        if (row.getNameItem()->checkState() == Qt::Checked)
            datasetsToRemove << row.getNameItem()->getDataset();

    std::sort(datasetsToRemove.begin(), datasetsToRemove.end(), [](auto datasetA, auto datasetB) -> bool {
        return datasetA->getDataHierarchyItem().getDepth() < datasetB->getDataHierarchyItem().getDepth();
    });

    std::reverse(datasetsToRemove.begin(), datasetsToRemove.end());

    return datasetsToRemove;
}

void DatasetsToRemoveModel::updateReadOnlyForAllNameItems()
{
    for (auto& row : Row::allRows)
        row.updateReadOnly();
}

void DatasetsToRemoveModel::setKeepDescendantsForAllNameItems()
{
    for (auto& row : Row::allRows)
        row.setKeepDescendants(settings().getMiscellaneousSettings().getKeepDescendantsAfterRemovalAction().isChecked());
}

void DatasetsToRemoveModel::checkAll()
{
    for (auto& row : Row::allRows)
        row.getNameItem()->setCheckState(Qt::Checked);
}

void DatasetsToRemoveModel::updateCheckStatesForAllNameItems()
{
    for (auto& row : Row::allRows)
        row.getNameItem()->updateCheckState();
}
