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
    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetChanged));
    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetSelectionChanged));
    _eventListener.registerDataEventByType(ClusterType, [this, updateActions](hdps::DataEvent* dataEvent) {
        if (!_clusters.isValid())
            return;

        if (dataEvent->getDataset() != _clusters)
            return;

        switch (dataEvent->getType()) {
            case EventType::DatasetAdded:
            case EventType::DatasetChanged:
            case EventType::DatasetSelectionChanged:
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
