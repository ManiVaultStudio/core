#include "InfoAction.h"

#include "event/Event.h"

using namespace hdps;
using namespace hdps::gui;

InfoAction::InfoAction(QObject* parent, Clusters& clusters) :
    GroupAction(parent, true),
    _clusters(&clusters),
    _numberOfClustersAction(this, "Number of clusters"),
    _clustersAction(this, clusters)
{
    setText("Clusters");

    _numberOfClustersAction.setEnabled(false);
    _numberOfClustersAction.setToolTip("The number of clusters");

    const auto updateActions = [this]() -> void {
        if (!_clusters.isValid())
            return;

        _numberOfClustersAction.setString(QString::number(_clusters->getClusters().size()));
    };

    _eventListener.setEventCore(Application::core());
    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DataAdded));
    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DataChanged));
    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DataSelectionChanged));
    _eventListener.registerDataEventByType(ClusterType, [this, updateActions](hdps::DataEvent* dataEvent) {
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
