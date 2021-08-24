#include "ClusterAction.h"
#include "CreateClusterDialog.h"

#include "DataHierarchyItem.h"
#include "PointData.h"
#include "ClusterData.h"

using namespace hdps;
using namespace hdps::gui;

ClusterAction::ClusterAction(QObject* parent, hdps::CoreInterface* core, const QString& datasetName) :
    WidgetAction(parent),
    EventListener(),
    _core(core),
    _inputDataHierarchyItem(nullptr),
    _clusterDataHierarchyItem(nullptr),
    _createClusterAction(this, "Create cluster")
{
    setText("Cluster");
    setEventCore(_core);

    _inputDataHierarchyItem = _core->getDataHierarchyItem(datasetName);

    const auto updateCreateClusterAction = [this]() -> void {
        auto& points    = _inputDataHierarchyItem->getDataset<Points>();
        auto& selection = dynamic_cast<Points&>(points.getSelection());

        _createClusterAction.setEnabled(!selection.indices.empty());
    };

    registerDataEventByType(PointType, [this, updateCreateClusterAction](hdps::DataEvent* dataEvent) {
        if (dataEvent->dataSetName != _inputDataHierarchyItem->getDatasetName())
            return;

        switch (dataEvent->getType()) {
            case EventType::SelectionChanged:
            {
                updateCreateClusterAction();
                break;
            }

            default:
                break;
        }
    });

    connect(&_createClusterAction, &TriggerAction::triggered, this, [this]() {
        if (_clusterDataHierarchyItem == nullptr) {
            const auto clusterDatasetName = _core->addData("Cluster", "Cluster", _inputDataHierarchyItem->getDatasetName());
            _clusterDataHierarchyItem = _core->getDataHierarchyItem(clusterDatasetName);
        }

        auto& createClusterDialog = CreateClusterDialog(_core, _inputDataHierarchyItem->getDatasetName());
        qDebug() << createClusterDialog.exec();

        /*
        auto& points            = _inputDataHierarchyItem->getDataset<Points>();
        auto& selection         = dynamic_cast<Points&>(points.getSelection());
        auto& clusterDataset    = _clusterDataHierarchyItem->getDataset<Clusters>();

        

        Cluster cluster;

        cluster._name   = createClusterDialog.getNameAction().getString();
        cluster._color  = createClusterDialog.getColorAction().getColor();
        cluster.indices = selection.indices;

        clusterDataset.addCluster(cluster);
        */
    });

    updateCreateClusterAction();
}

QMenu* ClusterAction::getContextMenu(QWidget* parent /*= nullptr*/)
{
    auto menu = new QMenu(text(), parent);

    menu->addAction(&_createClusterAction);

    return menu;
}

const std::vector<std::uint32_t>& ClusterAction::getSelectedIndices() const
{
    auto& points    = _inputDataHierarchyItem->getDataset<Points>();
    auto& selection = dynamic_cast<Points&>(points.getSelection());

    return selection.indices;
}
