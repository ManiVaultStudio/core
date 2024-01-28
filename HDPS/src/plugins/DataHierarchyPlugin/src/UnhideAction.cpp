// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "UnhideAction.h"

#include <Application.h>

#include <util/Icon.h>

#include <widgets/HierarchyWidget.h>

#include <QHeaderView>

using namespace mv;
using namespace mv::gui;

UnhideAction::UnhideAction(QObject* parent, const QString& title) :
    GroupAction(parent, title),
    _treeModel(this),
    _filterModel(this),
    _treeAction(this, "List"),
    _triggersGroupAction(this, "Triggers"),
    _selectedAction(this, "Unhide Selected"),
    _allAction(this, "Unhide All"),
    _icon(Application::getIconFont("FontAwesome").getIcon("eye"))
{
    setShowLabels(false);
    setPopupSizeHint(QSize(300, 300));

    getBadge().setEnabled(true);

    _filterModel.getVisibilityFilterAction().setSelectedOptions({ "Hidden" });

    _treeAction.initialize(&_treeModel, &_filterModel, "Hidden dataset");

    addAction(&_treeAction, -1, [this](WidgetAction* action, QWidget* widget) -> void {
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

        treeView->setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::Location), true);
        treeView->setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::DatasetId), true);
        treeView->setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::SourceDatasetId), true);
        treeView->setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::Progress), true);
        treeView->setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::GroupIndex), true);
        treeView->setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::IsGroup), true);
        treeView->setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::IsLocked), true);
        treeView->setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::IsDerived), true);

        auto treeViewHeader = treeView->header();

        treeViewHeader->setStretchLastSection(false);

        treeViewHeader->setSectionResizeMode(static_cast<int>(AbstractDataHierarchyModel::Column::Name), QHeaderView::Stretch);
        treeViewHeader->setSectionResizeMode(static_cast<int>(AbstractDataHierarchyModel::Column::IsVisible), QHeaderView::ResizeToContents);

        const auto resizeSections = [this, treeViewHeader]() -> void {
            if (_filterModel.rowCount() >= 1)
                treeViewHeader->resizeSections(QHeaderView::ResizeMode::ResizeToContents);
        };

        resizeSections();

        connect(&_filterModel, &QAbstractItemModel::rowsInserted, widget, resizeSections);
        connect(&_filterModel, &QAbstractItemModel::rowsRemoved, widget, resizeSections);
        connect(&_filterModel, &QAbstractItemModel::layoutChanged, widget, resizeSections);

        const auto updateTriggerActionsReadOnly = [this, treeView]() -> void {
            _selectedAction.setEnabled(!treeView->selectionModel()->selectedRows().isEmpty());
            _allAction.setEnabled(_filterModel.rowCount() >= 1);
        };

        updateTriggerActionsReadOnly();

        connect(&_filterModel, &QSortFilterProxyModel::rowsInserted, treeView, updateTriggerActionsReadOnly);
        connect(&_filterModel, &QSortFilterProxyModel::rowsRemoved, treeView, updateTriggerActionsReadOnly);
        connect(&_filterModel, &QSortFilterProxyModel::layoutChanged, treeView, updateTriggerActionsReadOnly);

        connect(treeView->selectionModel(), &QItemSelectionModel::selectionChanged, widget, updateTriggerActionsReadOnly);

        connect(&_selectedAction, &TriggerAction::triggered, widget, [this, treeView]() -> void {
            QModelIndexList persistentModelIndexes;

            for (auto rowIndex : treeView->selectionModel()->selectedRows())
                persistentModelIndexes << _filterModel.mapToSource(rowIndex.siblingAtColumn(static_cast<int>(AbstractDataHierarchyModel::Column::IsVisible)));

            for (auto persistentModelIndex : persistentModelIndexes)
                _treeModel.setData(persistentModelIndex, true, Qt::EditRole);
        });

        connect(&_allAction, &TriggerAction::triggered, widget, [this, treeView]() -> void {
            QModelIndexList persistentModelIndexes;

            for (int rowIndex = 0; rowIndex < _filterModel.rowCount(); rowIndex++)
                persistentModelIndexes << _filterModel.mapToSource(_filterModel.index(rowIndex, static_cast<int>(AbstractDataHierarchyModel::Column::IsVisible)));

            for (auto persistentModelIndex : persistentModelIndexes)
                _treeModel.setData(persistentModelIndex, true, Qt::EditRole);
        });
    });

    addAction(&_triggersGroupAction);

    _triggersGroupAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::NoLabelInGroup);

    _triggersGroupAction.addAction(&_selectedAction);
    _triggersGroupAction.addAction(&_allAction);

    const auto updateReadOnlyAndIcon = [this]() -> void {
        const auto mayUnhide    = _filterModel.rowCount() >= 1;
        const auto color        = qApp->palette().highlight().color();

        setEnabled(mayUnhide);
        setIcon(mayUnhide ? createIconWithNumberBadgeOverlay(_icon, 0, 0, color, color) : _icon);
    };

    updateReadOnlyAndIcon();

    connect(&_filterModel, &QSortFilterProxyModel::modelReset, this, updateReadOnlyAndIcon);
    connect(&_filterModel, &QSortFilterProxyModel::rowsInserted, this, updateReadOnlyAndIcon);
    connect(&_filterModel, &QSortFilterProxyModel::rowsRemoved, this, updateReadOnlyAndIcon);
    connect(&_filterModel, &QSortFilterProxyModel::layoutChanged, this, updateReadOnlyAndIcon);
}
