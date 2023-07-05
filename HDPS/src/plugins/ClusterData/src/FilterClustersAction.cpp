// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "FilterClustersAction.h"
#include "ClusterData.h"
#include "ClustersAction.h"
#include "ClustersFilterModel.h"
#include "ClustersActionWidget.h"

#include <Application.h>

#include <QHBoxLayout>

FilterClustersAction::FilterClustersAction(ClustersActionWidget* clustersActionWidget) :
    WidgetAction(clustersActionWidget, "Filter Clusters"),
    _clustersActionWidget(clustersActionWidget),
    _nameFilterAction(this, "Name filter")
{
    setIcon(Application::getIconFont("FontAwesome").getIcon("filter"));

    _nameFilterAction.setToolTip("Filter clusters by name (case-insensitive)");
    _nameFilterAction.setPlaceHolderString("Filter by name...");
    _nameFilterAction.setSearchMode(true);

    // Update the name filter in the filter model
    const auto updateNameFilter = [this]() -> void {
        _clustersActionWidget->getFilterModel().setNameFilter(_nameFilterAction.getString());
    };

    // Update the filter model filter when the name filter action string changes
    connect(&_nameFilterAction, &StringAction::stringChanged, this, updateNameFilter);

    // Do an initial update of the model name filter
    updateNameFilter();

    // Update read only status
    const auto updateReadOnly = [this]() -> void {
        setEnabled(_clustersActionWidget->getFilterModel().rowCount() >= 1);
    };

    // Update read only status when the model selection or layout changes
    connect(&_clustersActionWidget->getFilterModel(), &QAbstractItemModel::layoutChanged, this, updateReadOnly);

    // Do an initial update of the read only status
    updateReadOnly();
}

FilterClustersAction::Widget::Widget(QWidget* parent, FilterClustersAction* filterClustersAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, filterClustersAction)
{
    auto layout = new QHBoxLayout();

    layout->addWidget(filterClustersAction->getNameFilterAction().createWidget(this));

    setLayout(layout);
}
