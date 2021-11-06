#include "InfoAction.h"

using namespace hdps;
using namespace hdps::gui;

InfoAction::InfoAction(QObject* parent,const QString& datasetName) :
    GroupAction(parent, true),
    EventListener(),
    _clusters(datasetName),
    _numberOfClustersAction(this, "Number of clusters"),
    _clustersAction(this, datasetName)
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

        if (dataEvent->dataSetName != _clusters->getName())
            return;

        switch (dataEvent->getType()) {
            case EventType::DataAdded:
            case EventType::DataChanged:
            case EventType::SelectionChanged:
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
