// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "RefreshClustersAction.h"
#include "ClusterData.h"
#include "ClustersAction.h"
#include "ClustersFilterModel.h"
#include "ClustersActionWidget.h"

#include <Application.h>

RefreshClustersAction::RefreshClustersAction(ClustersActionWidget* clustersActionWidget) :
    TriggerAction(clustersActionWidget, "Refresh"),
    _clustersActionWidget(clustersActionWidget)
{
    setToolTip("Refresh clusters");
    setIconByName("sync");

    // Request a refresh from the clusters action when the action is triggered
    connect(this, &TriggerAction::triggered, this, [this]() {
        _clustersActionWidget->getClustersAction().invalidateClusters();
    });

    // Update read only status
    const auto updateReadOnly = [this]() -> void {
        setEnabled(_clustersActionWidget->getClustersAction().getClustersModel().rowCount() >= 1);
    };

    // Update read only status when the model selection or layout changes
    connect(&_clustersActionWidget->getClustersAction().getClustersModel(), &QAbstractItemModel::layoutChanged, this, updateReadOnly);

    // Do an initial update of the read only status
    updateReadOnly();
}
