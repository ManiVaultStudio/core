#include "RefreshClustersAction.h"
#include "ClusterData.h"
#include "ClustersAction.h"
#include "ClustersFilterModel.h"
#include "ClustersActionWidget.h"

#include <Application.h>

RefreshClustersAction::RefreshClustersAction(ClustersActionWidget* clustersActionWidget) :
    TriggerAction(clustersActionWidget),
    _clustersActionWidget(clustersActionWidget)
{
    setText("");
    setToolTip("Refresh clusters");
    setIcon(Application::getIconFont("FontAwesome").getIcon("sync"));

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
