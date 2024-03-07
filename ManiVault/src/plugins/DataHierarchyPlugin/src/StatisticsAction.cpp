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
    _rawDataGroupAction(this, "Raw data"),
    _rawDataCountGroupAction(this, "Raw data count"),
    _rawDataTreeAction(this, "Raw data overview"),
    _datasetsGroupAction(this, "Datasets"),
    _datasetsTreeAction(this, "Datasets overview"),
    _selectionsGroupAction(this, "Selections"),
    _selectionsTreeAction(this, "Selections")
{
    setIconByName("chart-bar");
    setPopupSizeHint(QSize(0, 0));
    setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    setLabelSizingType(GroupAction::LabelSizingType::Auto);

    _rawDataTreeAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _datasetsTreeAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _selectionsTreeAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);

    _rawDataTreeAction.setIconByName("bars");
    _datasetsTreeAction.setIconByName("bars");
    _selectionsTreeAction.setIconByName("bars");

    _rawDataTreeAction.initialize(&_rawDataModel, &_rawDataFilterModel, "Raw data set");
    _datasetsTreeAction.initialize(&_datasetsModel, &_datasetsFilterModel, "Dataset");
    _selectionsTreeAction.initialize(&_selectionsModel, &_selectionsFilterModel, "Selection");

    _rawDataTreeAction.setPopupSizeHint(QSize(450, 400));
    _datasetsTreeAction.setPopupSizeHint(QSize(200, 400));
    _selectionsTreeAction.setPopupSizeHint(QSize(350, 400));

    _rawDataGroupAction.setShowLabels(false);
    _datasetsGroupAction.setShowLabels(false);
    _selectionsGroupAction.setShowLabels(false);

    addAction(&_rawDataGroupAction);
    addAction(&_datasetsGroupAction);
    addAction(&_selectionsGroupAction);

    _rawDataCountGroupAction.setShowLabels(false);

    _rawDataCountGroupAction.addAction(&_rawDataModel.getCountAction());
    _rawDataCountGroupAction.addAction(&_rawDataTreeAction, -1, [this](WidgetAction* action, QWidget* widget) -> void {
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

        for (int columnIndex = 0; columnIndex < _rawDataModel.columnCount(); ++columnIndex)
            treeViewHeader->setSectionResizeMode(columnIndex, QHeaderView::ResizeToContents);

        treeViewHeader->resizeSections(QHeaderView::ResizeMode::ResizeToContents);
    });

    _rawDataGroupAction.setDefaultWidgetFlag(GroupAction::WidgetFlag::NoMargins);
    _rawDataGroupAction.setShowLabels(false);

    _rawDataGroupAction.addAction(&_rawDataCountGroupAction);
    _rawDataGroupAction.addAction(&_rawDataModel.getOverallSizeAction());

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

        for (int columnIndex = 0; columnIndex < _datasetsModel.columnCount(); ++columnIndex)
            treeViewHeader->setSectionResizeMode(columnIndex, QHeaderView::ResizeToContents);

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

        treeView->setColumnHidden(static_cast<int>(SelectionsModel::Column::ID), true);
        treeView->setColumnHidden(static_cast<int>(SelectionsModel::Column::RawDataType), true);

        auto treeViewHeader = treeView->header();

        treeViewHeader->setStretchLastSection(true);

        for (int columnIndex = 0; columnIndex < _selectionsModel.columnCount(); ++columnIndex)
            treeViewHeader->setSectionResizeMode(columnIndex, QHeaderView::ResizeToContents);

        treeViewHeader->resizeSections(QHeaderView::ResizeMode::ResizeToContents);
    });
}
