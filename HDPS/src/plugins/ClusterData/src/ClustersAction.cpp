#include "ClustersAction.h"
#include "ClustersModel.h"
#include "DataHierarchyItem.h"
#include "ClusterData.h"
#include "PointData.h"

#include <QTreeView>
#include <QHeaderView>
#include <QVBoxLayout>

using namespace hdps;
using namespace hdps::gui;

ClustersAction::ClustersAction(QObject* parent, hdps::CoreInterface* core, const QString& datasetName) :
    WidgetAction(parent),
    EventListener(),
    _core(core),
    _dataHierarchyItem(nullptr)
{
    setText("Clusters");
    setEventCore(_core);

    _dataHierarchyItem = _core->getDataHierarchyItem(datasetName);

    registerDataEventByType(ClusterType, [this](hdps::DataEvent* dataEvent) {
        if (dataEvent->dataSetName != _dataHierarchyItem->getDatasetName())
            return;

        if (dataEvent->getType() == EventType::DataChanged)
            emit clustersChanged(getClusters());
    });
}

std::vector<Cluster>* ClustersAction::getClusters()
{
    return &_dataHierarchyItem->getDataset<Clusters>().getClusters();
}

void ClustersAction::selectPoints(const std::vector<std::uint32_t>& indices)
{
    auto parentDataHierarchyItem = _core->getDataHierarchyItem(_dataHierarchyItem->getParent());
    
    auto& points    = parentDataHierarchyItem->getDataset<Points>();
    auto& selection = dynamic_cast<Points&>(points.getSelection());
    
    selection.indices = indices;

    _core->notifySelectionChanged(parentDataHierarchyItem->getDatasetName());
}

void ClustersAction::removeClusters(const QVector<Cluster*>& clusters)
{
    for (auto& cluster : clusters)
        _dataHierarchyItem->getDataset<Clusters>().removeCluster(*cluster);

    _core->notifyDataChanged(_dataHierarchyItem->getDatasetName());
}

ClustersAction::Widget::Widget(QWidget* parent, ClustersAction* clustersAction, const hdps::gui::WidgetActionWidget::State& state) :
    WidgetActionWidget(parent, clustersAction, state),
    _colorAction(this, "Color"),
    _removeAction(this, "Remove")
{
    _removeAction.setEnabled(false);

    auto clustersTreeView = new QTreeView();

    clustersTreeView->setFixedHeight(250);
    clustersTreeView->setRootIsDecorated(false);
    clustersTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    clustersTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    clustersTreeView->header()->setStretchLastSection(true);
    clustersTreeView->setFocusPolicy(Qt::ClickFocus);

    auto mainLayout = new QVBoxLayout();

    mainLayout->setMargin(0);
    mainLayout->addWidget(clustersTreeView);

    setLayout(mainLayout);

    const auto selectionChangedHandler = [this, clustersAction, clustersTreeView]() -> void {

        // Point indices that need to be selected
        std::vector<std::uint32_t> selectionIndices;

        const auto selectedRows = clustersTreeView->selectionModel()->selectedRows();

        // Gather point indices for selection
        for (auto selectedIndex : selectedRows) {
            auto cluster = static_cast<Cluster*>(selectedIndex.internalPointer());
            selectionIndices.insert(selectionIndices.end(), cluster->indices.begin(), cluster->indices.end());
        }

        // Remove duplicates
        std::sort(selectionIndices.begin(), selectionIndices.end());
        selectionIndices.erase(unique(selectionIndices.begin(), selectionIndices.end()), selectionIndices.end());

        // Select points
        clustersAction->selectPoints(selectionIndices);

        // Update state of the remove action
        _removeAction.setEnabled(!selectedRows.isEmpty());

        QColor color = Qt::gray;

        const auto canSelectColor = selectedRows.count() == 1;

        if (canSelectColor)
            color = static_cast<Cluster*>(selectedRows.first().internalPointer())->_color;

        _colorAction.setEnabled(canSelectColor);
        _colorAction.setColor(color);
    };

    //selectionChangedHandler();

    const auto updateClustersListView = [this, clustersAction, clustersTreeView, selectionChangedHandler]() -> void {
        clustersTreeView->setModel(new ClustersModel(this, clustersAction->getClusters()));

        connect(clustersTreeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, [this, selectionChangedHandler](const QItemSelection& selected, const QItemSelection& deselected) {
            selectionChangedHandler();
        });
    };

    connect(clustersAction, &ClustersAction::clustersChanged, this, [this, updateClustersListView](std::vector<Cluster>* clusters) {
        updateClustersListView();
    });

    updateClustersListView();

    auto toolbarLayout = new QHBoxLayout();

    mainLayout->addLayout(toolbarLayout);

    toolbarLayout->addWidget(_colorAction.createWidget(this), 1);
    toolbarLayout->addWidget(_removeAction.createWidget(this));

    connect(&_removeAction, &TriggerAction::triggered, this, [this, clustersAction, clustersTreeView, updateClustersListView]() {
        const auto selectedRows = clustersTreeView->selectionModel()->selectedRows();

        QVector<Cluster*> clusters;

        // Gather point indices for selection
        for (auto selectedIndex : selectedRows)
            clusters << static_cast<Cluster*>(selectedIndex.internalPointer());

        if (clusters.isEmpty())
            return;

        clustersAction->removeClusters(clusters);
    });

    /*
    connect(&_colorAction, &ColorAction::colorChanged, this, [this, clustersAction, clustersTreeView, updateClustersListView]() {
        const auto selectedRows = clustersTreeView->selectionModel()->selectedRows();

        QVector<Cluster*> clusters;

        // Gather point indices for selection
        for (auto selectedIndex : selectedRows)
            clusters << static_cast<Cluster*>(selectedIndex.internalPointer());

        if (clusters.isEmpty())
            return;

        clustersAction->removeClusters(clusters);
    });
    */
}
