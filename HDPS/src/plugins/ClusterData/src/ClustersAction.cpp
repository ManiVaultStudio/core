#include "ClustersAction.h"
#include <DataHierarchyItem.h>
#include "ClusterData.h"

#include <PointData.h>
#include <event/Event.h>

#include <QTreeView>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QVariant>
#include <QFile>
#include <QFileDialog>

using namespace hdps;
using namespace hdps::gui;

ClustersAction::ClustersAction(QObject* parent, Dataset<Clusters> clusters) :
    WidgetAction(parent),
    _clusters(clusters),
    _clustersModel()
{
    setText("Clusters");

    // Update the clusters model to reflect the changes in the clusters set
    connect(&_clusters, &Dataset<Clusters>::dataChanged, this, [this]() {
        _clustersModel.setClusters(*getClusters());
    });

    const auto updateClusters = [this]() -> void {
        _clusters->getClusters() = _clustersModel.getClusters();
        Application::core()->notifyDataChanged(*_clusters);
    };

    connect(&_clustersModel, &QAbstractItemModel::dataChanged, this, [this, updateClusters](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>()) {
        if (!_clusters.isValid())
            return;

        updateClusters();
    });

    connect(&_clustersModel, &QAbstractItemModel::layoutChanged, this, [this, updateClusters](const QList<QPersistentModelIndex> &parents = QList<QPersistentModelIndex>(), QAbstractItemModel::LayoutChangeHint hint = QAbstractItemModel::NoLayoutChangeHint) {
        if (!_clusters.isValid())
            return;

        updateClusters();
    });
}

std::vector<Cluster>* ClustersAction::getClusters()
{
    if (!_clusters.isValid())
        return nullptr;

    return &_clusters->getClusters();
}

Dataset<Clusters>& ClustersAction::getClustersDataset()
{
    return _clusters;
}

void ClustersAction::createSubset(const QString& datasetName)
{
    _clusters->getParent()->createSubset("Clusters");
}

void ClustersAction::removeClustersById(const QStringList& ids)
{
    _clustersModel.removeClustersById(ids);
}

ClustersModel& ClustersAction::getClustersModel()
{
    return _clustersModel;
}

ClustersAction::Widget::Widget(QWidget* parent, ClustersAction* clustersAction) :
    WidgetActionWidget(parent, clustersAction),
    _filterModel(),
    _selectionModel(&_filterModel),
    _removeClustersAction(*clustersAction, _filterModel, _selectionModel),
    _mergeClustersAction(*clustersAction, _filterModel, _selectionModel),
    _filterClustersAction(this, _filterModel, _selectionModel),
    _selectClustersAction(this, _filterModel, _selectionModel),
    _subsetAction(this, *clustersAction, _filterModel, _selectionModel)
{
    // Configure filter model
    _filterModel.setSourceModel(&clustersAction->getClustersModel());
    _filterModel.setDynamicSortFilter(true);
    _filterModel.setFilterKeyColumn(static_cast<std::int32_t>(ClustersModel::Column::Name));

    auto clustersTreeView = new QTreeView();

    // Configure tree view
    clustersTreeView->setModel(&_filterModel);
    clustersTreeView->setSelectionModel(&_selectionModel);
    clustersTreeView->setFixedHeight(300);
    clustersTreeView->setRootIsDecorated(false);
    clustersTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    clustersTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    clustersTreeView->setSortingEnabled(true);
    clustersTreeView->sortByColumn(static_cast<std::int32_t>(ClustersModel::Column::Name), Qt::SortOrder::AscendingOrder);

    // Configure header view
    auto header = clustersTreeView->header();

    header->setStretchLastSection(false);
    header->hideSection(static_cast<std::int32_t>(ClustersModel::Column::ID));

    header->setMinimumSectionSize(20);

    header->resizeSection(static_cast<std::int32_t>(ClustersModel::Column::Color), 20);
    header->resizeSection(static_cast<std::int32_t>(ClustersModel::Column::Name), 200);
    header->resizeSection(static_cast<std::int32_t>(ClustersModel::Column::NumberOfIndices), 100);

    header->setSectionResizeMode(static_cast<std::int32_t>(ClustersModel::Column::Color), QHeaderView::Fixed);
    header->setSectionResizeMode(static_cast<std::int32_t>(ClustersModel::Column::Name), QHeaderView::Stretch);
    header->setSectionResizeMode(static_cast<std::int32_t>(ClustersModel::Column::NumberOfIndices), QHeaderView::Fixed);
    
    const auto selectionChangedHandler = [this, clustersAction, clustersTreeView]() -> void {

        // Get selected row
        const auto selectedRows = clustersTreeView->selectionModel()->selectedRows();

        // Indices of the selected clusters
        std::vector<std::uint32_t> currentClusterSelectionIndices;

        // Clear and reserve the selection indices
        currentClusterSelectionIndices.reserve(clustersAction->getClusters()->size());

        // Gather point indices for selection
        for (auto selectedIndex : selectedRows) {
            auto cluster = static_cast<Cluster*>(_filterModel.mapToSource(selectedIndex).internalPointer());

            // Add selected index
            currentClusterSelectionIndices.push_back(selectedIndex.row());
        }

        // Select points
        clustersAction->getClustersDataset()->setSelectionIndices(currentClusterSelectionIndices);

        // Notify others that the cluster selection has changed
        Application::core()->notifyDataSelectionChanged(clustersAction->getClustersDataset());
    };

    connect(clustersTreeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, [this, selectionChangedHandler](const QItemSelection& selected, const QItemSelection& deselected) {
        selectionChangedHandler();
    });
    
    // Clear the selection when the layout of the model changes
    connect(clustersTreeView->model(), &QAbstractItemModel::layoutChanged, this, [this, clustersTreeView](const QList<QPersistentModelIndex> &parents = QList<QPersistentModelIndex>(), QAbstractItemModel::LayoutChangeHint hint = QAbstractItemModel::NoLayoutChangeHint) {
        clustersTreeView->selectionModel()->reset();
    });

    connect(clustersTreeView, &QTreeView::doubleClicked, this, [this, clustersAction, clustersTreeView](const QModelIndex& index) {
        const auto colorColumn = static_cast<std::int32_t>(ClustersModel::Column::Color);

        if (index.column() != colorColumn)
            return;

        // Get sibling color model index and the current cluster color
        const auto colorIndex   = _filterModel.mapToSource(index).siblingAtColumn(colorColumn);
        const auto currentColor = colorIndex.data(Qt::EditRole).value<QColor>();
        
        // Update the clusters model with the new color
        clustersAction->getClustersModel().setData(colorIndex, QColorDialog::getColor(currentColor));
    });

    auto toolbarLayout = new QHBoxLayout();

    toolbarLayout->addWidget(_removeClustersAction.createWidget(this), 1);
    toolbarLayout->addWidget(_mergeClustersAction.createWidget(this), 1);

    auto mainLayout = new QVBoxLayout();

    mainLayout->setMargin(0);
    mainLayout->addWidget(clustersTreeView);

    mainLayout->addWidget(_filterClustersAction.createWidget(this));
    mainLayout->addWidget(_selectClustersAction.createWidget(this));
    mainLayout->addLayout(toolbarLayout);
    mainLayout->addWidget(_subsetAction.createWidget(this));

    setLayout(mainLayout);
}
