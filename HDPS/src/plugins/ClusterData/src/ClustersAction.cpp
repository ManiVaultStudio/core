#include "ClustersAction.h"
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
    _dataHierarchyItem(nullptr),
    _clustersModel(this)
{
    setText("Clusters");
    setEventCore(_core);

    _dataHierarchyItem = _core->getDataHierarchyItem(datasetName);

    registerDataEventByType(ClusterType, [this](hdps::DataEvent* dataEvent) {
        if (dataEvent->dataSetName != _dataHierarchyItem->getDatasetName())
            return;

        if (dataEvent->getType() == EventType::DataChanged)
            _clustersModel.setClusters(getClusters());
    });

    connect(&_clustersModel, &QAbstractItemModel::dataChanged, this, [this](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>()) {
        Q_ASSERT(_dataHierarchyItem != nullptr);

        _core->notifyDataChanged(_dataHierarchyItem->getDatasetName());
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

void ClustersAction::removeClustersById(const QStringList& ids)
{
    Q_ASSERT(_dataHierarchyItem != nullptr);
    Q_ASSERT(!ids.isEmpty());

    if (ids.isEmpty())
        return;

    _dataHierarchyItem->getDataset<Clusters>().removeClustersById(ids);

    _core->notifyDataChanged(_dataHierarchyItem->getDatasetName());
}

ClustersModel& ClustersAction::getClustersModel()
{
    return _clustersModel;
}

ClustersAction::Widget::Widget(QWidget* parent, ClustersAction* clustersAction, const hdps::gui::WidgetActionWidget::State& state) :
    WidgetActionWidget(parent, clustersAction, state),
    _colorAction(this, "Color"),
    _removeAction(this, "Remove")
{
    _removeAction.setEnabled(false);

    auto clustersTreeView = new QTreeView();

    clustersTreeView->setFixedHeight(150);
    clustersTreeView->setRootIsDecorated(false);
    clustersTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    clustersTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    clustersTreeView->setModel(&clustersAction->getClustersModel());
    clustersTreeView->setColumnHidden(static_cast<std::int32_t>(ClustersModel::Column::ID), true);
    clustersTreeView->header()->setStretchLastSection(true);

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
            selectionIndices.insert(selectionIndices.end(), cluster->_indices.begin(), cluster->_indices.end());
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

        //QSignalBlocker colorActionBlocker(&_colorAction);

        _colorAction.setColor(color);
    };

    //selectionChangedHandler();

    connect(clustersTreeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, [this, selectionChangedHandler](const QItemSelection& selected, const QItemSelection& deselected) {
        selectionChangedHandler();
    });

    connect(clustersTreeView->model(), &QAbstractItemModel::modelAboutToBeReset, this, [this, clustersTreeView]() {
        _cacheClusterSelection = clustersTreeView->selectionModel()->selection();
    });

    connect(clustersTreeView->model(), &QAbstractItemModel::modelReset, this, [this, clustersTreeView]() {
        //clustersTreeView->selectionModel()->select(_cacheClusterSelection, QItemSelectionModel::SelectionFlag::Rows | QItemSelectionModel::SelectionFlag::ClearAndSelect);
    });

    auto toolbarLayout = new QHBoxLayout();

    mainLayout->addLayout(toolbarLayout);

    toolbarLayout->addWidget(_colorAction.createWidget(this), 1);
    toolbarLayout->addWidget(_removeAction.createWidget(this));

    connect(&_removeAction, &TriggerAction::triggered, this, [this, clustersAction, clustersTreeView]() {
        const auto selectedRows = clustersTreeView->selectionModel()->selectedRows();

        QStringList clusterIds;

        for (auto selectedIndex : selectedRows)
            clusterIds << selectedIndex.siblingAtColumn(static_cast<std::int32_t>(ClustersModel::Column::ID)).data(Qt::DisplayRole).toString();

        clustersAction->removeClustersById(clusterIds);
    });

    connect(&_colorAction, &ColorAction::colorChanged, this, [this, clustersAction, clustersTreeView]() {
        const auto selectedRows = clustersTreeView->selectionModel()->selectedRows();

        if (selectedRows.count() != 1)
            return;

        const auto newColor = _colorAction.getColor();

        auto cluster = static_cast<Cluster*>(selectedRows.first().internalPointer());
        
        if (newColor != cluster->_color)
            clustersTreeView->model()->setData(selectedRows.first(), newColor, Qt::DecorationRole);
    });
}
