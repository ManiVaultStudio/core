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
    _editAndCreateClusterAction(this, "create")
{
    setText("Cluster");
    setEventCore(_core);

    _editAndCreateClusterAction.setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("edit"));

    _inputDataHierarchyItem = _core->getDataHierarchyItem(datasetName);

    const auto updateCreateClusterAction = [this]() -> void {
        auto& points    = _inputDataHierarchyItem->getDataset<Points>();
        auto& selection = dynamic_cast<Points&>(points.getSelection());

        _editAndCreateClusterAction.setEnabled(!selection.indices.empty());
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

    connect(&_editAndCreateClusterAction, &TriggerAction::triggered, this, [this]() {
        if (_clusterDataHierarchyItem == nullptr) {
            const auto clusterDatasetName = _core->addData("Cluster", "Cluster", _inputDataHierarchyItem->getDatasetName());
            _clusterDataHierarchyItem = _core->getDataHierarchyItem(clusterDatasetName);
        }

        auto& createClusterDialog = CreateClusterDialog(_core, _inputDataHierarchyItem->getDatasetName());

        createClusterDialog.setModal(true);
        createClusterDialog.exec();
    });

    updateCreateClusterAction();
}

QMenu* ClusterAction::getContextMenu(QWidget* parent /*= nullptr*/)
{
    auto menu = new QMenu(text(), parent);

    menu->addAction(&_editAndCreateClusterAction);

    return menu;
}

const std::vector<std::uint32_t>& ClusterAction::getSelectedIndices() const
{
    auto& points    = _inputDataHierarchyItem->getDataset<Points>();
    auto& selection = dynamic_cast<Points&>(points.getSelection());

    return selection.indices;
}
