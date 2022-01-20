#include "InfoAction.h"

#include "event/Event.h"

using namespace hdps;
using namespace hdps::gui;

InfoAction::InfoAction(QObject* parent, Clusters& clusters) :
    GroupAction(parent, false),
    EventListener(),
    _clusters(&clusters),
    _numberOfClustersAction(this, "Number of clusters"),
    _clustersAction(this, clusters)
{
    setText("Clusters");
    setEventCore(Application::core());

    _numberOfClustersAction.setEnabled(false);
    _numberOfClustersAction.setToolTip("The number of clusters");

    _numberOfClustersAction.setMayReset(false);
    _clustersAction.setMayReset(false);

    const auto updateActions = [this]() -> void {
        if (!_clusters.isValid())
            return;

        _numberOfClustersAction.setString(QString::number(_clusters->getClusters().size()));
    };

    registerDataEventByType(ClusterType, [this, updateActions](hdps::DataEvent* dataEvent) {
        if (!_clusters.isValid())
            return;

        if (dataEvent->getDataset() != _clusters)
            return;

        switch (dataEvent->getType()) {
            case EventType::DataAdded:
            case EventType::DataChanged:
            case EventType::DataSelectionChanged:
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
