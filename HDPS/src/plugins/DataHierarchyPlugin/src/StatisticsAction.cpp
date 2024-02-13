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
    _rawDataFilterModel(),
    _datasetsModel(),
    _datasetsFilterModel(),
    _selectionsModel(),
    _selectionsFilterModel(),
    _rawDataGroupAction(this, "Raw data group"),
    _overallRawDataSizeAction(this, "Overall raw data size"),
    _refreshRawDataAction(this, "Refresh raw data"),
    _rawDataTreeAction(this, "Raw data"),
    _datasetsGroupAction(this, "Datasets group"),
    _datasetsTreeAction(this, "Datasets"),
    _selectionsGroupAction(this, "Selections group"),
    _selectionsTreeAction(this, "Selections")
{
    setIconByName("chart-bar");
    setPopupSizeHint(QSize(200, 0));
    setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    setLabelSizingType(GroupAction::LabelSizingType::Auto);
    setShowLabels(false);

    _overallRawDataSizeAction.setEnabled(false);

    _refreshRawDataAction.setIconByName("sync");
    _refreshRawDataAction.setDefaultWidgetFlags(TriggerAction::WidgetFlag::Icon);

    _rawDataTreeAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _datasetsTreeAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _selectionsTreeAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);

    _rawDataTreeAction.setIconByName("list-ul");
    _datasetsTreeAction.setIconByName("list-ul");
    _selectionsTreeAction.setIconByName("list-ul");

    _rawDataTreeAction.initialize(&_rawDataModel, &_rawDataFilterModel, "Raw data");
    _datasetsTreeAction.initialize(&_datasetsModel, &_datasetsFilterModel, "Dataset");
    _selectionsTreeAction.initialize(&_selectionsModel, &_selectionsFilterModel, "Selection");

    _rawDataTreeAction.setPopupSizeHint(QSize(400, 400));
    _datasetsTreeAction.setPopupSizeHint(QSize(250, 400));
    _selectionsTreeAction.setPopupSizeHint(QSize(250, 400));

    _rawDataGroupAction.setShowLabels(false);
    _datasetsGroupAction.setShowLabels(false);
    _selectionsGroupAction.setShowLabels(false);

    addAction(&_rawDataGroupAction);
    addAction(&_datasetsGroupAction);
    addAction(&_selectionsGroupAction);

    refreshOverallRawDataSize();

    connect(&_datasetsModel, &QAbstractItemModel::rowsInserted, this, &StatisticsAction::refreshOverallRawDataSize);
    connect(&_datasetsModel, &QAbstractItemModel::rowsRemoved, this, &StatisticsAction::refreshOverallRawDataSize);
    connect(&_datasetsModel, &QAbstractItemModel::layoutChanged, this, &StatisticsAction::refreshOverallRawDataSize);

    _rawDataGroupAction.addAction(&_overallRawDataSizeAction);
    _rawDataGroupAction.addAction(&_refreshRawDataAction);
    _rawDataGroupAction.addAction(&_rawDataTreeAction, -1, [this](WidgetAction* action, QWidget* widget) -> void {
        auto hierarchyWidget = widget->findChild<HierarchyWidget*>("HierarchyWidget");

        Q_ASSERT(hierarchyWidget != nullptr);

        if (hierarchyWidget == nullptr)
            return;

        auto treeView = widget->findChild<QTreeView*>("TreeView");

        Q_ASSERT(treeView != nullptr);

        if (treeView == nullptr)
            return;

        treeView->setRootIsDecorated(false);

        auto treeViewHeader = treeView->header();

        treeViewHeader->setStretchLastSection(true);

        treeViewHeader->setSectionResizeMode(static_cast<int>(RawDataModel::Column::Name), QHeaderView::ResizeToContents);
        treeViewHeader->setSectionResizeMode(static_cast<int>(RawDataModel::Column::Type), QHeaderView::ResizeToContents);
        treeViewHeader->setSectionResizeMode(static_cast<int>(RawDataModel::Column::Size), QHeaderView::ResizeToContents);

        treeViewHeader->resizeSections(QHeaderView::ResizeMode::ResizeToContents);
    });

    _datasetsGroupAction.addAction(&_datasetsModel.getCountAction());
    _datasetsGroupAction.addAction(&_datasetsTreeAction, -1, [this](WidgetAction* action, QWidget* widget) -> void {
        auto hierarchyWidget = widget->findChild<HierarchyWidget*>("HierarchyWidget");

        Q_ASSERT(hierarchyWidget != nullptr);

        if (hierarchyWidget == nullptr)
            return;

        auto treeView = widget->findChild<QTreeView*>("TreeView");

        Q_ASSERT(treeView != nullptr);

        if (treeView == nullptr)
            return;

        treeView->setRootIsDecorated(false);

        treeView->setColumnHidden(static_cast<int>(DatasetsModel::Column::DatasetId), true);
        treeView->setColumnHidden(static_cast<int>(DatasetsModel::Column::RawDataName), true);
        treeView->setColumnHidden(static_cast<int>(DatasetsModel::Column::SourceDatasetId), true);

        auto treeViewHeader = treeView->header();

        treeViewHeader->setStretchLastSection(true);

        treeViewHeader->setSectionResizeMode(static_cast<int>(DatasetsModel::Column::Name), QHeaderView::ResizeToContents);
        treeViewHeader->setSectionResizeMode(static_cast<int>(DatasetsModel::Column::DatasetId), QHeaderView::ResizeToContents);
        treeViewHeader->setSectionResizeMode(static_cast<int>(DatasetsModel::Column::RawDataName), QHeaderView::ResizeToContents);
        treeViewHeader->setSectionResizeMode(static_cast<int>(DatasetsModel::Column::SourceDatasetId), QHeaderView::ResizeToContents);

        treeViewHeader->resizeSections(QHeaderView::ResizeMode::ResizeToContents);
    });

    _selectionsGroupAction.addAction(&_selectionsModel.getCountAction());
    _selectionsGroupAction.addAction(&_selectionsTreeAction, -1, [this](WidgetAction* action, QWidget* widget) -> void {
        auto hierarchyWidget = widget->findChild<HierarchyWidget*>("HierarchyWidget");

        Q_ASSERT(hierarchyWidget != nullptr);

        if (hierarchyWidget == nullptr)
            return;

        auto treeView = widget->findChild<QTreeView*>("TreeView");

        Q_ASSERT(treeView != nullptr);

        if (treeView == nullptr)
            return;

        treeView->setRootIsDecorated(false);

        treeView->setColumnHidden(static_cast<int>(SelectionsModel::Column::Name), true);

        auto treeViewHeader = treeView->header();

        treeViewHeader->setStretchLastSection(true);

        treeViewHeader->setSectionResizeMode(static_cast<int>(SelectionsModel::Column::Name), QHeaderView::ResizeToContents);

        treeViewHeader->resizeSections(QHeaderView::ResizeMode::ResizeToContents);
    });

    connect(&mv::data(), &AbstractDataManager::rawDataAdded, this, &StatisticsAction::refreshRawData);
    connect(&mv::data(), &AbstractDataManager::rawDataRemoved, this, &StatisticsAction::refreshRawData);

    connect(&_refreshRawDataAction, &TriggerAction::triggered, this, &StatisticsAction::refreshRawData);

    refreshRawData();
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
