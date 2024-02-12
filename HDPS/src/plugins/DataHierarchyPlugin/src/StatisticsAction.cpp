// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StatisticsAction.h"

#include <CoreInterface.h>
#include <Application.h>
#include <Dataset.h>
#include <Set.h>

#include <util/Icon.h>

#include <widgets/HierarchyWidget.h>

#include <QHeaderView>

using namespace mv;
using namespace mv::gui;

StatisticsAction::StatisticsAction(QObject* parent, const QString& title) :
    GroupAction(parent, title),
    _rawDataModel(),
    _datasetsModel(),
    _selectionsModel(),
    _showIdsAction(this, "Show identifiers"),
    _rawDataTreeAction(this, "Raw data"),
    _rawDataGroupAction(this, "Raw data group"),
    _overallRawDataSizeAction(this, "Overall raw data size"),
    _refreshRawDataAction(this, "Refresh raw data"),
    _datasetsTreeAction(this, "Datasets"),
    _selectionsTreeAction(this, "Selections")
{
    setIconByName("chart-bar");
    //setShowLabels(false);
    setPopupSizeHint(QSize(300, 600));
    setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    setLabelSizingType(GroupAction::LabelSizingType::Auto);

    _rawDataModel.setHorizontalHeaderLabels({ "Name", "Size" });
    _datasetsModel.setHorizontalHeaderLabels({ "Name", "ID", "Raw data name" });
    _selectionsModel.setHorizontalHeaderLabels({ "Name", "ID", "Raw data name" });

    _rawDataGroupAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::NoLabelInGroup);
    _overallRawDataSizeAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::NoLabelInGroup);

    _overallRawDataSizeAction.setEnabled(false);

    _rawDataTreeAction.setDefaultWidgetFlag(TreeAction::WidgetFlag::FilterToolbar, false);
    _datasetsTreeAction.setDefaultWidgetFlag(TreeAction::WidgetFlag::FilterToolbar, false);
    _selectionsTreeAction.setDefaultWidgetFlag(TreeAction::WidgetFlag::FilterToolbar, false);

    _rawDataTreeAction.initialize(&_rawDataModel, nullptr, "Raw data");
    _datasetsTreeAction.initialize(&_datasetsModel, nullptr, "Dataset");
    _selectionsTreeAction.initialize(&_selectionsModel, nullptr, "Selection set");

    addAction(&_showIdsAction);

    addAction(&_rawDataTreeAction, -1, [this](WidgetAction* action, QWidget* widget) -> void {
        auto hierarchyWidget = widget->findChild<HierarchyWidget*>("HierarchyWidget");

        Q_ASSERT(hierarchyWidget != nullptr);

        if (hierarchyWidget == nullptr)
            return;

        hierarchyWidget->getToolbarAction().setVisible(false);

        auto treeView = widget->findChild<QTreeView*>("TreeView");

        Q_ASSERT(treeView != nullptr);

        if (treeView == nullptr)
            return;

        treeView->setRootIsDecorated(false);

        auto treeViewHeader = treeView->header();

        treeViewHeader->setStretchLastSection(true);
    });

    refreshOverallRawDataSize();

    connect(&_datasetsModel, &QAbstractItemModel::rowsInserted, this, &StatisticsAction::refreshOverallRawDataSize);
    connect(&_datasetsModel, &QAbstractItemModel::rowsRemoved, this, &StatisticsAction::refreshOverallRawDataSize);
    connect(&_datasetsModel, &QAbstractItemModel::layoutChanged, this, &StatisticsAction::refreshOverallRawDataSize);

    _rawDataGroupAction.addAction(&_overallRawDataSizeAction);
    _rawDataGroupAction.addAction(&_refreshRawDataAction);

    addAction(&_rawDataGroupAction);

    addAction(&_datasetsTreeAction, -1, [this](WidgetAction* action, QWidget* widget) -> void {
        auto hierarchyWidget = widget->findChild<HierarchyWidget*>("HierarchyWidget");

        Q_ASSERT(hierarchyWidget != nullptr);

        if (hierarchyWidget == nullptr)
            return;

        hierarchyWidget->getToolbarAction().setVisible(false);

        auto treeView = widget->findChild<QTreeView*>("TreeView");

        Q_ASSERT(treeView != nullptr);

        if (treeView == nullptr)
            return;

        treeView->setRootIsDecorated(false);

        auto treeViewHeader = treeView->header();

        treeViewHeader->setStretchLastSection(true);

        treeViewHeader->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        treeViewHeader->setSectionResizeMode(1, QHeaderView::ResizeToContents);
        treeViewHeader->setSectionResizeMode(2, QHeaderView::ResizeToContents);

        const auto resizeSections = [this, treeViewHeader]() -> void {
            if (_datasetsModel.rowCount() >= 1)
                treeViewHeader->resizeSections(QHeaderView::ResizeMode::ResizeToContents);
        };

        resizeSections();

        connect(&_datasetsModel, &QAbstractItemModel::rowsInserted, widget, resizeSections);
        connect(&_datasetsModel, &QAbstractItemModel::rowsRemoved, widget, resizeSections);
        connect(&_datasetsModel, &QAbstractItemModel::layoutChanged, widget, resizeSections);

        const auto updateColumnVisibility = [this, treeView]() -> void {
            const auto columnHidden = !_showIdsAction.isChecked();

            for (int columnIndex = 1; columnIndex < _selectionsModel.rowCount(); ++columnIndex)
                treeView->setColumnHidden(columnIndex, columnHidden);
        };

        updateColumnVisibility();

        connect(&_showIdsAction, &ToggleAction::toggled, treeView, updateColumnVisibility);
    });

    addAction(&_selectionsTreeAction, -1, [this](WidgetAction* action, QWidget* widget) -> void {
        auto hierarchyWidget = widget->findChild<HierarchyWidget*>("HierarchyWidget");

        Q_ASSERT(hierarchyWidget != nullptr);

        if (hierarchyWidget == nullptr)
            return;

        hierarchyWidget->getToolbarAction().setVisible(false);

        auto treeView = widget->findChild<QTreeView*>("TreeView");

        Q_ASSERT(treeView != nullptr);

        if (treeView == nullptr)
            return;

        treeView->setRootIsDecorated(false);
        auto treeViewHeader = treeView->header();

        treeViewHeader->setStretchLastSection(true);

        treeViewHeader->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        treeViewHeader->setSectionResizeMode(1, QHeaderView::ResizeToContents);
        treeViewHeader->setSectionResizeMode(2, QHeaderView::ResizeToContents);

        const auto resizeSections = [this, treeViewHeader]() -> void {
            if (_selectionsModel.rowCount() >= 1)
                treeViewHeader->resizeSections(QHeaderView::ResizeMode::ResizeToContents);
        };

        resizeSections();

        connect(&_selectionsModel, &QAbstractItemModel::rowsInserted, widget, resizeSections);
        connect(&_selectionsModel, &QAbstractItemModel::rowsRemoved, widget, resizeSections);
        connect(&_selectionsModel, &QAbstractItemModel::layoutChanged, widget, resizeSections);

        const auto updateColumnVisibility = [this, treeView]() -> void {
            const auto columnHidden = !_showIdsAction.isChecked();

            for (int columnIndex = 1; columnIndex < _selectionsModel.rowCount(); ++columnIndex)
                treeView->setColumnHidden(columnIndex, columnHidden);
        };

        updateColumnVisibility();

        connect(&_showIdsAction, &ToggleAction::toggled, treeView, updateColumnVisibility);
    });

    connect(&mv::data(), &AbstractDataManager::rawDataAdded, this, &StatisticsAction::refreshRawData);
    connect(&mv::data(), &AbstractDataManager::rawDataRemoved, this, &StatisticsAction::refreshRawData);

    connect(&mv::data(), &AbstractDataManager::datasetAdded, this, &StatisticsAction::refreshDatasets);
    connect(&mv::data(), &AbstractDataManager::datasetRemoved, this, &StatisticsAction::refreshDatasets);

    connect(&mv::data(), &AbstractDataManager::selectionAdded, this, &StatisticsAction::refreshSelections);
    connect(&mv::data(), &AbstractDataManager::selectionRemoved, this, &StatisticsAction::refreshSelections);

    connect(&_refreshRawDataAction, &TriggerAction::triggered, this, &StatisticsAction::refreshRawData);
    //connect(&_refreshRawDataAction, &TriggerAction::triggered, this, &StatisticsAction::refreshDatasets);
    //connect(&_refreshRawDataAction, &TriggerAction::triggered, this, &StatisticsAction::refreshRawData);

    refreshRawData();
    refreshDatasets();
    refreshSelections();
}

void StatisticsAction::refreshRawData()
{
    _rawDataModel.setRowCount(0);

    for (const auto& rawDataName : mv::data().getRawDataNames())
        _rawDataModel.appendRow({ new QStandardItem(rawDataName), new QStandardItem(util::getNoBytesHumanReadable(mv::data().getRawDataSize(rawDataName))) });

    refreshOverallRawDataSize();
}

void StatisticsAction::refreshOverallRawDataSize()
{
    _overallRawDataSizeAction.setString(util::getNoBytesHumanReadable(mv::data().getOverallRawDataSize()));
}

void StatisticsAction::refreshDatasets()
{
    _datasetsModel.setRowCount(0);

    for (const auto& dataset : mv::data().getAllDatasets())
        _datasetsModel.appendRow({ new QStandardItem(dataset->getGuiName()), new QStandardItem(dataset->getId()), new QStandardItem(dataset->getRawDataName()) });
}

void StatisticsAction::refreshSelections()
{
    _selectionsModel.setRowCount(0);

    for (const auto& selection : mv::data().getAllSelections())
        _selectionsModel.appendRow({ new QStandardItem(selection->getGuiName()), new QStandardItem(selection->getId()), new QStandardItem(selection->getRawDataName()) });
}

/*
auto hierarchyWidget = widget->findChild<HierarchyWidget*>("HierarchyWidget");

        Q_ASSERT(hierarchyWidget != nullptr);

        if (hierarchyWidget == nullptr)
            return;

        hierarchyWidget->getToolbarAction().setVisible(false);
        hierarchyWidget->setWindowIcon(Application::getIconFont("FontAwesome").getIcon("database"));

        auto treeView = widget->findChild<QTreeView*>("TreeView");

        Q_ASSERT(treeView != nullptr);

        if (treeView == nullptr)
            return;

        treeView->setRootIsDecorated(false);
        treeView->expandAll();

        treeView->setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::Name), true);
        treeView->setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::DatasetId), true);
        treeView->setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::RawDataId), true);
        treeView->setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::SourceDatasetId), true);
        treeView->setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::Progress), true);
        treeView->setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::SelectionGroupIndex), true);
        treeView->setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::IsGroup), true);
        treeView->setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::IsLocked), true);
        treeView->setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::IsDerived), true);
        treeView->setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::IsSubset), true);

        auto treeViewHeader = treeView->header();

        treeViewHeader->setStretchLastSection(false);

        treeViewHeader->setSectionResizeMode(static_cast<int>(AbstractDataHierarchyModel::Column::Location), QHeaderView::Stretch);
        treeViewHeader->setSectionResizeMode(static_cast<int>(AbstractDataHierarchyModel::Column::IsVisible), QHeaderView::ResizeToContents);

        const auto resizeSections = [this, treeViewHeader]() -> void {
            if (_listFilterModel.rowCount() >= 1)
                treeViewHeader->resizeSections(QHeaderView::ResizeMode::ResizeToContents);
        };

        resizeSections();

        connect(&_listFilterModel, &QAbstractItemModel::rowsInserted, widget, resizeSections);
        connect(&_listFilterModel, &QAbstractItemModel::rowsRemoved, widget, resizeSections);
        connect(&_listFilterModel, &QAbstractItemModel::layoutChanged, widget, resizeSections);

        const auto updateTriggerActionsReadOnly = [this, treeView]() -> void {
            _selectedAction.setEnabled(!treeView->selectionModel()->selectedRows().isEmpty());
            _allAction.setEnabled(_listFilterModel.rowCount() >= 1);
        };

        updateTriggerActionsReadOnly();

        connect(&_listFilterModel, &QSortFilterProxyModel::rowsInserted, treeView, updateTriggerActionsReadOnly);
        connect(&_listFilterModel, &QSortFilterProxyModel::rowsRemoved, treeView, updateTriggerActionsReadOnly);
        connect(&_listFilterModel, &QSortFilterProxyModel::layoutChanged, treeView, updateTriggerActionsReadOnly);

        connect(treeView->selectionModel(), &QItemSelectionModel::selectionChanged, widget, updateTriggerActionsReadOnly);

        connect(&_selectedAction, &TriggerAction::triggered, widget, [this, treeView]() -> void {
            QModelIndexList persistentModelIndexes;

            for (auto rowIndex : treeView->selectionModel()->selectedRows())
                persistentModelIndexes << _listFilterModel.mapToSource(rowIndex.siblingAtColumn(static_cast<int>(AbstractDataHierarchyModel::Column::IsVisible)));

            for (auto persistentModelIndex : persistentModelIndexes)
                _listModel.unhideItem(persistentModelIndex);
        });

        connect(&_allAction, &TriggerAction::triggered, widget, [this, treeView]() -> void {
            QModelIndexList persistentModelIndexes;

            for (int rowIndex = 0; rowIndex < _listFilterModel.rowCount(); rowIndex++)
                persistentModelIndexes << _listFilterModel.mapToSource(_listFilterModel.index(rowIndex, static_cast<int>(AbstractDataHierarchyModel::Column::IsVisible)));

            for (auto persistentModelIndex : persistentModelIndexes)
                _listModel.unhideItem(persistentModelIndex);
        });
*/