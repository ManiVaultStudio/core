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
    _treeAction(this, "List"),
    _listModel(this),
    _listFilterModel(this),
    _triggersGroupAction(this, "Triggers"),
    _selectedAction(this, "Unhide Selected"),
    _allAction(this, "Unhide All"),
    _icon(StyledIcon("eye-slash"))
{
    setIcon(_icon);
    setShowLabels(false);
    setPopupSizeHint(QSize(500, 300));
    setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);

    getBadge().setScale(0.4f);

    _treeAction.initialize(&_listModel, &_listFilterModel, "Hidden dataset");

    _listFilterModel.setSourceModel(&_listModel);
    _listFilterModel.getVisibilityFilterAction().setSelectedOptions({ "Hidden" });

    addAction(&_treeAction, -1, [this](WidgetAction* action, QWidget* widget) -> void {
        auto hierarchyWidget = widget->findChild<HierarchyWidget*>("HierarchyWidget");

        Q_ASSERT(hierarchyWidget != nullptr);

        if (hierarchyWidget == nullptr)
            return;

        hierarchyWidget->getToolbarAction().setVisible(false);
        hierarchyWidget->setWindowIcon(StyledIcon("database"));

        auto treeView = widget->findChild<QTreeView*>("TreeView");

        Q_ASSERT(treeView != nullptr);

        if (treeView == nullptr)
            return;

        treeView->setRootIsDecorated(false);
        treeView->expandAll();

        treeView->setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::Name), true);
        treeView->setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::DatasetId), true);
        treeView->setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::RawDataName), true);
        treeView->setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::RawDataSize), true);
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

            for (const auto& rowIndex : treeView->selectionModel()->selectedRows())
                persistentModelIndexes << _listFilterModel.mapToSource(rowIndex.siblingAtColumn(static_cast<int>(AbstractDataHierarchyModel::Column::IsVisible)));

            for (const auto& persistentModelIndex : persistentModelIndexes)
                _listModel.unhideItem(persistentModelIndex);
        });

        connect(&_allAction, &TriggerAction::triggered, widget, [this, treeView]() -> void {
            QModelIndexList persistentModelIndexes;

            for (int rowIndex = 0; rowIndex < _listFilterModel.rowCount(); rowIndex++)
                persistentModelIndexes << _listFilterModel.mapToSource(_listFilterModel.index(rowIndex, static_cast<int>(AbstractDataHierarchyModel::Column::IsVisible)));

            for (const auto& persistentModelIndex : persistentModelIndexes)
                _listModel.unhideItem(persistentModelIndex);
        });
    });

    addAction(&_triggersGroupAction);

    _triggersGroupAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::NoLabelInGroup);

    _triggersGroupAction.addAction(&_selectedAction);
    _triggersGroupAction.addAction(&_allAction);

    updateReadOnlyAndIcon();

    connect(&_listFilterModel, &QSortFilterProxyModel::modelReset, this, &UnhideAction::updateReadOnlyAndIcon);
    connect(&_listFilterModel, &QSortFilterProxyModel::rowsInserted, this, &UnhideAction::updateReadOnlyAndIcon);
    connect(&_listFilterModel, &QSortFilterProxyModel::rowsRemoved, this, &UnhideAction::updateReadOnlyAndIcon);
    connect(&_listFilterModel, &QSortFilterProxyModel::layoutChanged, this, &UnhideAction::updateReadOnlyAndIcon);
}

void UnhideAction::updateReadOnlyAndIcon()
{
    const auto numberOfHiddenDatasets = _listFilterModel.rowCount();
    const auto mayUnhide = numberOfHiddenDatasets >= 1;

    setEnabled(mayUnhide);

    auto& badge = getBadge();

    badge.setEnabled(mayUnhide);
    badge.setNumber(numberOfHiddenDatasets);
    badge.setBackgroundColor(qApp->palette().highlight().color());
}

bool UnhideAction::event(QEvent* event)
{
    if (event->type() == QEvent::ApplicationPaletteChange)
        updateReadOnlyAndIcon();

    return WidgetAction::event(event);
}
