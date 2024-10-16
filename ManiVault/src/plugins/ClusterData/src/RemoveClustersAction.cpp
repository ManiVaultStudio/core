// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "RemoveClustersAction.h"
#include "ClustersAction.h"
#include "ClustersFilterModel.h"
#include "ClusterData.h"
#include "ClustersActionWidget.h"

RemoveClustersAction::RemoveClustersAction(ClustersActionWidget* clustersActionWidget) :
    TriggerAction(clustersActionWidget, "Remove"),
    _clustersActionWidget(clustersActionWidget)
{
    setToolTip("Remove selected cluster(s)");
    setIconByName("trash");
    setDefaultWidgetFlags(TriggerAction::Icon);

    // Remove selected clusters when the action is triggered
    connect(this, &TriggerAction::triggered, this, [this]() {

        // Get the selected rows from the selection model
        const auto selectedRows = _clustersActionWidget->getSelectionModel().selectedRows();

        QStringList clusterIds;

        // Build a list of cluster identifiers to remove
        for (auto selectedIndex : selectedRows)
            clusterIds << _clustersActionWidget->getFilterModel().mapToSource(selectedIndex).siblingAtColumn(static_cast<std::int32_t>(ClustersModel::Column::ID)).data(Qt::DisplayRole).toString();

        // Remove the clusters
        _clustersActionWidget->getClustersAction().removeClustersById(clusterIds);
    });

    // Update action read only status
    const auto updateReadOnly = [this]() -> void {
        setEnabled(_clustersActionWidget->getClustersAction().getClustersModel().rowCount() >= 1 && _clustersActionWidget->getSelectionModel().hasSelection());
    };

    // Update action read only status when the item selection changes or the layout
    connect(&_clustersActionWidget->getSelectionModel(), &QItemSelectionModel::selectionChanged, this, updateReadOnly);
    connect(&_clustersActionWidget->getClustersAction().getClustersModel(), &QAbstractItemModel::layoutChanged, this, updateReadOnly);

    // Initialize read only status
    updateReadOnly();
}
