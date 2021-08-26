#include "InfoAction.h"

#include "DataHierarchyItem.h"
#include "ClusterData.h"

using namespace hdps;
using namespace hdps::gui;

InfoAction::InfoAction(QObject* parent, CoreInterface* core, const QString& datasetName) :
    GroupAction(parent, true),
    EventListener(),
    _core(core),
    _dataHierarchyItem(nullptr),
    _numberOfClustersAction(this, "Number of clusters"),
    _clustersAction(this, core, datasetName)
{
    setText("Clusters");
    setEventCore(_core);

    _dataHierarchyItem = _core->getDataHierarchyItem(datasetName);

    _numberOfClustersAction.setEnabled(false);
    _numberOfClustersAction.setToolTip("The number of clusters");

    const auto updateActions = [this]() -> void {
        auto& clusters = _dataHierarchyItem->getDataset<Clusters>();

        _numberOfClustersAction.setString(QString::number(clusters.getClusters().size()));
    };

    registerDataEventByType(ClusterType, [this, updateActions](hdps::DataEvent* dataEvent) {
        if (dataEvent->dataSetName != _dataHierarchyItem->getDatasetName())
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
