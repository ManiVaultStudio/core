// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SubsetAction.h"
#include "ClusterData.h"
#include "ClustersAction.h"
#include "ClustersFilterModel.h"
#include "ClustersActionWidget.h"

#include <QHBoxLayout>

SubsetAction::SubsetAction(ClustersActionWidget* clustersActionWidget) :
    WidgetAction(clustersActionWidget, "Subset"),
    _clustersActionWidget(clustersActionWidget),
    _subsetNameAction(this, "Subset name"),
    _createSubsetAction(this, "Create subset")
{
    setIconByName("crop");

    _subsetNameAction.setToolTip("Name of the subset");
    _subsetNameAction.setPlaceHolderString("Enter subset name here...");

    _createSubsetAction.setToolTip("Create the subset");

    // Get item selection for entire filter model
    const auto getItemSelection = [this]() -> QItemSelection {
        const auto numberOfItems    = _clustersActionWidget->getFilterModel().rowCount();
        const auto firstItemIndex   = _clustersActionWidget->getFilterModel().index(0, 0);
        const auto lastItem         = _clustersActionWidget->getFilterModel().index(numberOfItems - 1, 0);

        return QItemSelection(firstItemIndex, lastItem);
    };

    // Create the subset when the create subset action is triggered
    connect(&_createSubsetAction, &TriggerAction::triggered, this, [this, getItemSelection]() {
        _clustersActionWidget->getClustersAction().createSubsetFromSelection(_subsetNameAction.getString());
        _subsetNameAction.setString("");
    });

    // Update the read only status of the actions
    const auto updateReadOnly = [this]() -> void {
        const auto selectedRows         = _clustersActionWidget->getSelectionModel().selectedRows();
        const auto numberOfSelectedRows = selectedRows.count();

        setEnabled(numberOfSelectedRows >= 1);

        _createSubsetAction.setEnabled(!_subsetNameAction.getString().isEmpty());
    };

    // Update read only status when model selection changes
    connect(&_clustersActionWidget->getSelectionModel(), &QItemSelectionModel::selectionChanged, this, updateReadOnly);

    // Update read only status when the model layout changes
    connect(&_clustersActionWidget->getFilterModel(), &QAbstractItemModel::layoutChanged, this, updateReadOnly);

    // Update read only status when the subset name string changes
    connect(&_subsetNameAction, &StringAction::stringChanged, this, updateReadOnly);

    // Do initial read only updates
    updateReadOnly();
}

SubsetAction::Widget::Widget(QWidget* parent, SubsetAction* subsetAction) :
    WidgetActionWidget(parent, subsetAction)
{
    auto layout = new QHBoxLayout();

    setFixedWidth(250);

    layout->addWidget(subsetAction->getSubsetNameAction().createWidget(this));
    layout->addWidget(subsetAction->getCreateSubsetAction().createWidget(this));

    setLayout(layout);
}
