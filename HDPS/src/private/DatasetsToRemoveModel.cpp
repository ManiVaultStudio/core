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

QList<DatasetsToRemoveModel::Item*> DatasetsToRemoveModel::Item::allItems = QList<DatasetsToRemoveModel::Item*>();

DatasetsToRemoveModel::Item::Item(Dataset<DatasetImpl> dataset, DatasetsToRemoveModel& datasetsToRemoveModel, bool editable /*= false*/) :
    QStandardItem(),
    QObject(),
    _datasetsToRemoveModel(datasetsToRemoveModel),
    _dataset(dataset)
{
    Q_ASSERT(_dataset.isValid());

    if (!_dataset->isVisible())
        return;

    allItems << this;

    setEditable(editable);
    setAutoTristate(true);

    updateReadOnly();

    connect(&getDatasetRemoveModel().getKeepChildrenAction(), &ToggleAction::toggled, this, &Item::updateReadOnly);
    connect(&getDatasetRemoveModel().getAdvancedAction(), &ToggleAction::toggled, this, &Item::updateReadOnly);
}

DatasetsToRemoveModel::Item::~Item()
{
    allItems.removeOne(this);
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

    qDebug() << getDataset()->getGuiName() << hasParent;

    if (getDatasetRemoveModel().getKeepChildrenAction().isChecked())
        setEnabled(getDataset()->mayUnderive());
    else
        setEnabled(!hasParent);

    emitDataChanged();
}

DatasetsToRemoveModel::NameItem::NameItem(Dataset<DatasetImpl> dataset, DatasetsToRemoveModel& datasetsToRemoveModel) :
    Item(dataset, datasetsToRemoveModel, true)
{
    setCheckable(dataset->mayUnderive());

    updateCheckState();

    connect(&getDatasetRemoveModel().getKeepChildrenAction(), &ToggleAction::toggled, this, &NameItem::updateCheckState);

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

        case Qt::CheckStateRole:
        {
            Item::setData(getDataset()->mayUnderive() ? value : Qt::Unchecked, role);

            if (hasChildren()) {
                const auto numberOfChildren = rowCount();

                for (int childIndex = 0; childIndex < numberOfChildren; ++childIndex)
                    child(childIndex)->setData(value, role);
            }

            break;
        }

        default:
            Item::setData(value, role);
    }
}

void DatasetsToRemoveModel::NameItem::updateCheckState()
{
    if (getDataset()->mayUnderive())
        setCheckState(getDatasetRemoveModel().getKeepChildrenAction().isChecked() ? (QStandardItem::parent() == nullptr ? Qt::Checked : Qt::Unchecked) : Qt::Checked);
    else
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

DatasetsToRemoveModel::VisibleItem::VisibleItem(mv::Dataset<mv::DatasetImpl> dataset, DatasetsToRemoveModel& datasetsToRemoveModel) :
    Item(dataset, datasetsToRemoveModel)
{
    updateVisibility();

    connect(&getDatasetRemoveModel().getKeepChildrenAction(), &ToggleAction::toggled, this, &VisibleItem::updateVisibility);
    connect(&getDatasetRemoveModel().getAdvancedAction(), &ToggleAction::toggled, this, &VisibleItem::updateVisibility);
}

QVariant DatasetsToRemoveModel::VisibleItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return _visible;

        case Qt::DisplayRole:
            return data(Qt::EditRole).toBool() ? "Yes" : "No";

        case Qt::ToolTipRole:
            return "Item visible: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

void DatasetsToRemoveModel::VisibleItem::updateVisibility()
{
    if (!index().isValid())
        return;

    auto nameItem = model()->itemFromIndex(index().siblingAtColumn(static_cast<int>(Column::Name)));

    _visible = getDatasetRemoveModel().getAdvancedAction().isChecked() ? true : !nameItem->parent();

    emitDataChanged();
}

DatasetsToRemoveModel::InfoItem::InfoItem(mv::Dataset<mv::DatasetImpl> dataset, DatasetsToRemoveModel& datasetsToRemoveModel) :
    Item(dataset, datasetsToRemoveModel)
{
}

QVariant DatasetsToRemoveModel::InfoItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::DecorationRole:
        {
            if (!getDataset()->mayUnderive())
                return Application::getIconFont("FontAwesome").getIcon("info-circle");

            break;
        }

        case Qt::ToolTipRole:
        {
            if (!getDataset()->mayUnderive())
                return "Item must co-exists with its parent dataset";

            break;
        }

        default:
            break;
    }

    return Item::data(role);
}

DatasetsToRemoveModel::Row::Row(Dataset<DatasetImpl> dataset, DatasetsToRemoveModel& datasetsToRemoveModel) :
    QList<QStandardItem*>()
{
    append(new NameItem(dataset, datasetsToRemoveModel));
    append(new DatasetIdItem(dataset, datasetsToRemoveModel));
    append(new VisibleItem(dataset, datasetsToRemoveModel));
    append(new InfoItem(dataset, datasetsToRemoveModel));
}

DatasetsToRemoveModel::DatasetsToRemoveModel(QObject* parent) :
    QStandardItemModel(parent),
    _keepChildrenAction(this, "Keep children", false),
    _advancedAction(this, "Advanced", false)
{
    setColumnCount(static_cast<int>(Column::Count));

    _keepChildrenAction.setToolTip("If checked, children will not be removed and become orphans (placed at the root of the hierarchy)");
    _advancedAction.setToolTip("Whether to enable advanced remove configuration");
}

QVariant DatasetsToRemoveModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (static_cast<Column>(section))
    {
        case Column::Name:
            return NameItem::headerData(orientation, role);

        case Column::DatasetId:
            return DatasetIdItem::headerData(orientation, role);

        case Column::Visible:
            return VisibleItem::headerData(orientation, role);

        case Column::Info:
            return InfoItem::headerData(orientation, role);

        default:
            break;
    }

    return {};
}

void DatasetsToRemoveModel::setDatasets(mv::Datasets datasets)
{
    Item::allItems.clear();

    const auto printDatasets = [](QString message, Datasets datasets) -> void {
        qDebug() << "";
        qDebug() << message;

        for (auto dataset : datasets)
            qDebug() << dataset->getGuiName();
    };

    DataHierarchyItems selectedDataHierarchyItems, topLevelSelectedDataHierarchyItems;

    for (auto& selectedDataset : datasets)
        selectedDataHierarchyItems << &selectedDataset->getDataHierarchyItem();

    Datasets datasetsToRemove, topLevelDatasetsToRemove, descendantDatasetsToRemove;

    for (auto& topLevelDatasetCandidate : datasets)
        if (!topLevelDatasetCandidate->getDataHierarchyItem().isChildOf(selectedDataHierarchyItems))
            topLevelDatasetsToRemove << topLevelDatasetCandidate;

    printDatasets("Top-level datasets to remove", topLevelDatasetsToRemove);

    for (auto topLevelDatasetToRemove : topLevelDatasetsToRemove)
        for (auto descendantDataHierarchyItem : topLevelDatasetToRemove->getDataHierarchyItem().getChildren(true))
            descendantDatasetsToRemove << descendantDataHierarchyItem->getDataset();

    printDatasets("Descendant datasets to remove", descendantDatasetsToRemove);

    for (auto topLevelDatasetToRemove : topLevelDatasetsToRemove)
        topLevelSelectedDataHierarchyItems << &topLevelDatasetToRemove->getDataHierarchyItem();

    datasetsToRemove << topLevelDatasetsToRemove << descendantDatasetsToRemove;

    printDatasets("Datasets to remove", datasetsToRemove);

    std::sort(datasetsToRemove.begin(), datasetsToRemove.end(), [](auto datasetA, auto datasetB) -> bool {
        return datasetA->getDataHierarchyItem().getDepth() < datasetB->getDataHierarchyItem().getDepth();
    });

    printDatasets("Depth-sorted datasets to remove", datasetsToRemove);

    datasetsToRemove.erase(std::unique(datasetsToRemove.begin(), datasetsToRemove.end()), datasetsToRemove.end());

    printDatasets("After removing duplicates", datasetsToRemove);

    for (auto topLevelDatasetToRemove : topLevelDatasetsToRemove)
        appendRow(Row(topLevelDatasetToRemove, *this));

    for (auto descendantDatasetToRemove : descendantDatasetsToRemove) {
        const auto matches = match(index(0, static_cast<int>(Column::DatasetId), QModelIndex()), Qt::EditRole, descendantDatasetToRemove->getDataHierarchyItem().getParent()->getDataset()->getId(), -1, Qt::MatchFlag::MatchExactly | Qt::MatchFlag::MatchRecursive);

        if (matches.isEmpty())
            throw std::runtime_error("Parent data hierarchy item not found in model");

        itemFromIndex(matches.first().siblingAtColumn(static_cast<int>(Column::Name)))->appendRow(Row(descendantDatasetToRemove, *this));
    }

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

            itemFromIndex(matches.first().siblingAtColumn(static_cast<int>(Column::Name)))->appendRow(Row(dataset, *this));
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

    for (auto item : Item::allItems)
        if (item->checkState() == Qt::Checked)
            datasetsToRemove << item->getDataset();

    std::sort(datasetsToRemove.begin(), datasetsToRemove.end(), [](auto datasetA, auto datasetB) -> bool {
        return datasetA->getDataHierarchyItem().getDepth() < datasetB->getDataHierarchyItem().getDepth();
    });

    std::reverse(datasetsToRemove.begin(), datasetsToRemove.end());

    return datasetsToRemove;
}
