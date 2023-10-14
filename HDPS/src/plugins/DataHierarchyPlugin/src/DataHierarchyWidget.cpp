// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataHierarchyWidget.h"
#include "DataHierarchyWidgetContextMenu.h"

#include <models/DataHierarchyModel.h>
#include <models/DataHierarchyModelItem.h>
#include <Application.h>
#include <Set.h>
#include <Dataset.h>
#include <PluginFactory.h>
#include <AbstractDataHierarchyManager.h>
#include <widgets/Divider.h>
#include <actions/PluginTriggerAction.h>

#include <QDebug>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QMenu>
#include <QLabel>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QScopedPointer>

#include <stdexcept>

using namespace mv;
using namespace mv::util;
using namespace mv::plugin;
using namespace mv::gui;

/**
 * Tree view item delegate class
 * Qt natively does not support disabled items to be selected, this class solves that
 * When an item (dataset) is locked, merely the visual representation is changed and not the item flags (only appears disabled)
 */
class ItemDelegate : public QStyledItemDelegate {
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    explicit ItemDelegate(QObject* parent = nullptr) :
        QStyledItemDelegate(parent)
    {
    }

protected:

    /** Init the style option(s) for the item delegate (we override the options to paint disabled when locked) */
    void initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const override
    {
        QStyledItemDelegate::initStyleOption(option, index);

        if (index.siblingAtColumn(DataHierarchyModelItem::Column::IsLocked).data(Qt::EditRole).toBool())
            option->state &= ~QStyle::State_Enabled;
    }
};

DataHierarchyWidget::DataHierarchyWidget(QWidget* parent) :
    QWidget(parent),
    _model(this),
    _filterModel(this),
    _hierarchyWidget(this, "Dataset", _model, &_filterModel),
    _groupingAction(this, "Selection grouping", Application::core()->isDatasetGroupingEnabled()),
    _resetAction(this, "Reset")
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(&_hierarchyWidget, 1);

    setLayout(layout);

    _hierarchyWidget.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("database"));
    _hierarchyWidget.setNoItemsDescription(QString("Right-click > Import to load data into %1").arg(Application::getName()));
    
    auto& settingsGroupAction = _hierarchyWidget.getSettingsGroupAction();

    settingsGroupAction.setVisible(true);
    settingsGroupAction.setShowLabels(false);

    settingsGroupAction.addAction(&_groupingAction);
    settingsGroupAction.addAction(&_resetAction);

    auto& treeView = _hierarchyWidget.getTreeView();

    auto treeViewHeader = treeView.header();

    treeViewHeader->setStretchLastSection(false);
    treeViewHeader->setMinimumSectionSize(18);

    treeViewHeader->resizeSection(DataHierarchyModelItem::Column::Name, 180);
    treeViewHeader->resizeSection(DataHierarchyModelItem::Column::GroupIndex, 60);
    treeViewHeader->resizeSection(DataHierarchyModelItem::Column::Progress, 45);
    treeViewHeader->resizeSection(DataHierarchyModelItem::Column::IsGroup, treeViewHeader->minimumSectionSize());
    treeViewHeader->resizeSection(DataHierarchyModelItem::Column::IsAnalyzing, treeViewHeader->minimumSectionSize());
    treeViewHeader->resizeSection(DataHierarchyModelItem::Column::IsLocked, treeViewHeader->minimumSectionSize());

    treeViewHeader->setSectionResizeMode(DataHierarchyModelItem::Column::Name, QHeaderView::Interactive);
    treeViewHeader->setSectionResizeMode(DataHierarchyModelItem::Column::GUID, QHeaderView::Fixed);
    treeViewHeader->setSectionResizeMode(DataHierarchyModelItem::Column::GroupIndex, QHeaderView::Fixed);
    treeViewHeader->setSectionResizeMode(DataHierarchyModelItem::Column::Info, QHeaderView::Stretch);
    treeViewHeader->setSectionResizeMode(DataHierarchyModelItem::Column::Progress, QHeaderView::Fixed);
    treeViewHeader->setSectionResizeMode(DataHierarchyModelItem::Column::IsGroup, QHeaderView::Fixed);
    treeViewHeader->setSectionResizeMode(DataHierarchyModelItem::Column::IsAnalyzing, QHeaderView::Fixed);
    treeViewHeader->setSectionResizeMode(DataHierarchyModelItem::Column::IsLocked, QHeaderView::Fixed);

    treeView.setItemDelegate(new ItemDelegate());

    _groupingAction.setIconByName("object-group");
    _groupingAction.setToolTip("Enable/disable dataset grouping");

    connect(&_groupingAction, &ToggleAction::toggled, this, &DataHierarchyWidget::onGroupingActionToggled);

    connect(&_resetAction, &TriggerAction::triggered, &Application::core()->getDataManager(), &AbstractDataManager::reset);

    connect(&Application::core()->getDataHierarchyManager(), &AbstractDataHierarchyManager::itemAdded, this, [this](DataHierarchyItem& dataHierarchyItem) -> void {
        addDataHierarchyItem(dataHierarchyItem);
    });

    connect(&Application::core()->getDataHierarchyManager(), &AbstractDataHierarchyManager::itemAboutToBeRemoved, this, [this](const Dataset<DatasetImpl>& dataset) {
        _hierarchyWidget.getSelectionModel().clear();

        _model.removeDataHierarchyModelItem(getModelIndexByDataset(dataset));
    });

    connect(&_hierarchyWidget.getSelectionModel(), &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection& selected, const QItemSelection& deselected) {
        DataHierarchyItems dataHierarchyItems;

        for (const auto& index : _hierarchyWidget.getSelectedRows())
            dataHierarchyItems << _model.getItem(index, Qt::DisplayRole)->getDataHierarchyItem();

        Application::core()->getDataHierarchyManager().selectItems(dataHierarchyItems);
    });

    connect(&_hierarchyWidget.getTreeView(), &QTreeView::customContextMenuRequested, this, [this](const QPoint& position) {

        auto createContextMenu = [this, position]() -> void {
            Datasets datasets;

            for (const auto& selectedRow : _hierarchyWidget.getSelectedRows())
                datasets << _model.getItem(selectedRow, Qt::DisplayRole)->getDataHierarchyItem()->getDataset();

            QScopedPointer<DataHierarchyWidgetContextMenu> datasetsContextMenu(new DataHierarchyWidgetContextMenu(this, datasets));
            datasetsContextMenu->exec(_hierarchyWidget.getTreeView().viewport()->mapToGlobal(position));

            };

        // Get around possible signal-execution order complications
        QTimer::singleShot(10, createContextMenu);
    });

    for (const auto topLevelItem : Application::core()->getDataHierarchyManager().getTopLevelItems())
        addDataHierarchyItem(*topLevelItem);

    updateColumnsVisibility();
}

void DataHierarchyWidget::addDataHierarchyItem(DataHierarchyItem& dataHierarchyItem)
{
    auto dataset = dataHierarchyItem.getDataset();

    try {
        QModelIndex parentModelIndex;

        if (!dataHierarchyItem.hasParent())
            parentModelIndex = QModelIndex();
        else
            parentModelIndex = getModelIndexByDataset(dataHierarchyItem.getParent().getDataset());;

        _model.addDataHierarchyModelItem(parentModelIndex, dataHierarchyItem);

        connect(&dataHierarchyItem.getDataset()->getDatasetTask(), &DatasetTask::progressDescriptionChanged, this, [this, dataset](const QString& progressDescription) {
            _model.setData(getModelIndexByDataset(dataset).siblingAtColumn(DataHierarchyModelItem::Column::Info), progressDescription);
        });

        connect(&dataHierarchyItem.getDataset()->getDatasetTask(), &DatasetTask::progressChanged, this, [this, dataset](float progress) {
            _model.setData(getModelIndexByDataset(dataset).siblingAtColumn(DataHierarchyModelItem::Column::Progress), progress);
            _model.setData(getModelIndexByDataset(dataset).siblingAtColumn(DataHierarchyModelItem::Column::IsAnalyzing), progress > 0.0f);
        });

        connect(&dataHierarchyItem, &DataHierarchyItem::selectionChanged, this, [this, dataset](bool selection) {
            if (!selection)
                return;

            if (_hierarchyWidget.getSelectedRows().contains(getModelIndexByDataset(dataset)))
                return;

            _hierarchyWidget.getSelectionModel().select(_filterModel.mapFromSource(getModelIndexByDataset(dataset)), QItemSelectionModel::SelectionFlag::ClearAndSelect | QItemSelectionModel::SelectionFlag::Rows);
        });

        connect(&dataHierarchyItem, &DataHierarchyItem::lockedChanged, this, [this, dataset](bool locked) {
            emit _model.dataChanged(getModelIndexByDataset(dataset).siblingAtColumn(DataHierarchyModelItem::Column::Name), getModelIndexByDataset(dataset).siblingAtColumn(DataHierarchyModelItem::Column::IsLocked));
        });

        connect(&dataHierarchyItem, &DataHierarchyItem::visibilityChanged, this, [this](bool visible) {
            _filterModel.invalidate();
        });
        
        _filterModel.invalidate();

        connect(&dataHierarchyItem, &DataHierarchyItem::iconChanged, this, [this, dataset]() {
            emit _model.dataChanged(getModelIndexByDataset(dataset).siblingAtColumn(DataHierarchyModelItem::Column::Name), getModelIndexByDataset(dataset).siblingAtColumn(DataHierarchyModelItem::Column::Name));
        });

        /*
        const auto setModelItemExpansion = [this](const QModelIndex& modelIndex, bool expanded) -> void {
            //if (!modelIndex.isValid() || (expanded == _hierarchyWidget.getTreeView().isExpanded(modelIndex)))
            //    return;

            _hierarchyWidget.getTreeView().expand(modelIndex);

            QCoreApplication::processEvents();
        };

        connect(&dataHierarchyItem, &DataHierarchyItem::expandedChanged, this, [this, dataset, setModelItemExpansion](bool expanded) {
            setModelItemExpansion(getModelIndexByDataset(dataset), expanded);
        });

        setModelItemExpansion(getModelIndexByDataset(dataset), dataHierarchyItem.isExpanded());

        _hierarchyWidget.getTreeView().expand(getModelIndexByDataset(dataset));
        */
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to add %1 to the data hierarchy tree widget").arg(dataset->text()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to add %1 to the data hierarchy tree widget").arg(dataset->text()));
    }
}

QModelIndex DataHierarchyWidget::getModelIndexByDataset(const Dataset<DatasetImpl>& dataset)
{
    const auto modelIndices = _model.match(_model.index(0, 1), Qt::DisplayRole, dataset->getId(), 1, Qt::MatchFlag::MatchRecursive);

    if (modelIndices.isEmpty())
        throw new std::runtime_error(QString("'%1' not found in the data hierarchy model").arg(dataset->text()).toLatin1());

    return modelIndices.first();
}

void DataHierarchyWidget::onGroupingActionToggled(const bool& toggled)
{
    Application::core()->setDatasetGroupingEnabled(toggled);

    updateColumnsVisibility();
}

void DataHierarchyWidget::updateColumnsVisibility()
{
    auto& treeView = _hierarchyWidget.getTreeView();

    treeView.setColumnHidden(DataHierarchyModelItem::Column::GUID, true);
    treeView.setColumnHidden(DataHierarchyModelItem::Column::GroupIndex, !_groupingAction.isChecked());
}
