// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "InfoAction.h"

#include "event/Event.h"

using namespace hdps;
using namespace hdps::gui;

InfoAction::InfoAction(QObject* parent, Clusters& clusters) :
    GroupAction(parent, "Group", true),
    _clusters(&clusters),
    _numberOfClustersAction(this, "Number of clusters"),
    _clustersAction(this, clusters)
{
    setText("Clusters");

    addAction(&_numberOfClustersAction);
    addAction(&_clustersAction);
    
    _numberOfClustersAction.setEnabled(false);
    _numberOfClustersAction.setToolTip("The number of clusters");

    const auto updateActions = [this]() -> void {
        if (!_clusters.isValid())
            return;

        _numberOfClustersAction.setString(QString::number(_clusters->getClusters().size()));
    };

    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetAdded));
    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetDataChanged));
    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetDataSelectionChanged));
    _eventListener.registerDataEventByType(ClusterType, [this, updateActions](hdps::DatasetEvent* dataEvent) {
        if (!_clusters.isValid())
            return;

        if (dataEvent->getDataset() != _clusters)
            return;

        switch (dataEvent->getType()) {
            case EventType::DatasetAdded:
            case EventType::DatasetDataChanged:
            case EventType::DatasetDataSelectionChanged:
            {
                updateActions();
                break;
            }

            default:
                break;
        }
    });

    updateActions();
}
