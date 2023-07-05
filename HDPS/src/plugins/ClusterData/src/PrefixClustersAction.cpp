// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PrefixClustersAction.h"
#include "ClusterData.h"
#include "ClustersAction.h"
#include "ClustersFilterModel.h"
#include "ClustersActionWidget.h"

#include <Application.h>

PrefixClustersAction::PrefixClustersAction(ClustersAction& clustersAction) :
    TriggerAction(&clustersAction, "Prefix"),
    _clustersAction(clustersAction),
    _prefixAction(this, "Name prefix", "id_"),
    _applyAction(this, "Apply")
{
    setToolTip("Prefix clusters");
    setIcon(Application::getIconFont("FontAwesome").getIcon("paragraph"));

    // Set cluster prefix in model
    _clustersAction.getClustersModel().setClusterPrefix(_prefixAction.getString());

    // Update read only status
    const auto updateReadOnly = [this]() -> void {
        setEnabled(_clustersAction.getClustersModel().rowCount() >= 1);
    };

    // Update read only status when the model selection or layout changes
    connect(&_clustersAction.getClustersModel(), &QAbstractItemModel::layoutChanged, this, updateReadOnly);
    
    // Do an initial update of the read only status
    updateReadOnly();

    // Update read only status of apply action
    const auto updateApplyActionReadOnly = [this]() -> void {
        _applyAction.setEnabled(!_clustersAction.getClustersModel().doAllClusterNamesStartWith(_prefixAction.getString()) || _prefixAction.isResettable());
    };

    // Update read only status of apply action when the prefix string changes
    connect(&_prefixAction, &StringAction::stringChanged, this, updateApplyActionReadOnly);

    // Apply the prefix when the apply action is triggered
    connect(&_applyAction, &TriggerAction::triggered, this, [this, updateApplyActionReadOnly]() {

        // Get cluster name prefix
        const auto clusterNamePrefix = _prefixAction.getString();

        // Prefix the clusters in the model
        _clustersAction.getClustersModel().setClusterPrefix(clusterNamePrefix);

        // Update read only status
        updateApplyActionReadOnly();
    });

    // Update read only status of apply action when the data in the clusters model changes
    connect(&_clustersAction.getClustersModel(), &QAbstractItemModel::dataChanged, this, [updateApplyActionReadOnly](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>()) {
        if (topLeft.column() == static_cast<std::int32_t>(ClustersModel::Column::Name))
            updateApplyActionReadOnly();
    });

    // Update read only status of apply action when the data layout in the clusters model changes
    connect(&_clustersAction.getClustersModel(), &QAbstractItemModel::layoutChanged, this, updateApplyActionReadOnly);

    // Perform initial update of the read only status
    updateApplyActionReadOnly();
}

PrefixClustersAction::Widget::Widget(QWidget* parent, PrefixClustersAction* prefixClustersAction) :
    WidgetActionWidget(parent, prefixClustersAction)
{
    auto layout = new QHBoxLayout();

    layout->addWidget(prefixClustersAction->getPrefixAction().createLabelWidget(this));
    layout->addWidget(prefixClustersAction->getPrefixAction().createWidget(this));
    layout->addWidget(prefixClustersAction->getApplyAction().createWidget(this));

    setLayout(layout);
}
