// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SelectClustersAction.h"
#include "ClusterData.h"
#include "ClustersAction.h"
#include "ClustersFilterModel.h"
#include "ClustersActionWidget.h"

#include <QHBoxLayout>

SelectClustersAction::SelectClustersAction(ClustersActionWidget* clustersActionWidget) :
    WidgetAction(clustersActionWidget, "Select Clusters"),
    _clustersActionWidget(clustersActionWidget),
    _selectAllAction(this, "All"),
    _selectNoneAction(this, "None"),
    _selectInvertAction(this, "Invert")
{
    setIconByName("arrow-pointer");

    _selectAllAction.setToolTip("Select all clusters");
    _selectNoneAction.setToolTip("De-select all clusters");
    _selectInvertAction.setToolTip("Invert the cluster selection");

    // Get item selection for entire filter model
    const auto getItemSelection = [this]() -> QItemSelection {
        const auto numberOfItems    = _clustersActionWidget->getFilterModel().rowCount();
        const auto firstItemIndex   = _clustersActionWidget->getFilterModel().index(0, 0);
        const auto lastItem         = _clustersActionWidget->getFilterModel().index(numberOfItems - 1, 0);

        return QItemSelection(firstItemIndex, lastItem);
    };

    // Select all clusters when the select all action is triggered
    connect(&_selectAllAction, &TriggerAction::triggered, this, [this, getItemSelection]() {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        {
            _clustersActionWidget->getSelectionModel().select(getItemSelection(), QItemSelectionModel::Rows | QItemSelectionModel::Select);
        }
        QApplication::restoreOverrideCursor();
    });

    // De-select all clusters when the select none action is triggered
    connect(&_selectNoneAction, &TriggerAction::triggered, this, [this, getItemSelection]() {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        {
            _clustersActionWidget->getSelectionModel().select(getItemSelection(), QItemSelectionModel::Rows | QItemSelectionModel::Deselect);
        }
        QApplication::restoreOverrideCursor();
    });

    // Invert the cluster selection when the select invert action is triggered
    connect(&_selectInvertAction, &TriggerAction::triggered, this, [this, getItemSelection]() {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        {
            _clustersActionWidget->getSelectionModel().select(getItemSelection(), QItemSelectionModel::Rows | QItemSelectionModel::Toggle);
        }
        QApplication::restoreOverrideCursor();
    });

    // Update select actions read only status
    const auto updateSelectActionsReadOnly = [this]() -> void {
        const auto numberOfRows         = _clustersActionWidget->getFilterModel().rowCount();
        const auto selectedRows         = _clustersActionWidget->getSelectionModel().selectedRows();
        const auto numberOfSelectedRows = selectedRows.count();
        const auto hasSelection         = !selectedRows.isEmpty();

        _selectAllAction.setEnabled(numberOfSelectedRows != numberOfRows);
        _selectNoneAction.setEnabled(numberOfSelectedRows >= 1);
        _selectInvertAction.setEnabled(numberOfRows >= 1 );
    };

    // Update select actions read only status when the model selection or layout changes
    connect(&_clustersActionWidget->getSelectionModel(), &QItemSelectionModel::selectionChanged, this, updateSelectActionsReadOnly);
    connect(&_clustersActionWidget->getFilterModel(), &QAbstractItemModel::layoutChanged, this, updateSelectActionsReadOnly);

    // Do an initial update of the actions read only status
    updateSelectActionsReadOnly();

    // Update read only status
    const auto updateReadOnly = [this]() -> void {
        setEnabled(_clustersActionWidget->getFilterModel().rowCount() >= 1);
    };

    // Update read only status when the model selection or layout changes
    connect(&_clustersActionWidget->getFilterModel(), &QAbstractItemModel::layoutChanged, this, updateReadOnly);

    // Do an initial update of the read only status
    updateReadOnly();
}

SelectClustersAction::Widget::Widget(QWidget* parent, SelectClustersAction* selectClustersAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, selectClustersAction)
{
    auto layout = new QHBoxLayout();

    layout->addWidget(selectClustersAction->getSelectAllAction().createWidget(this));
    layout->addWidget(selectClustersAction->getSelectNoneAction().createWidget(this));
    layout->addWidget(selectClustersAction->getSelectInvertAction().createWidget(this));

    setLayout(layout);
}
