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
            _clustersModel.setClusters(*getClusters());
    });

    connect(&_clustersModel, &QAbstractItemModel::dataChanged, this, [this](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>()) {
        Q_ASSERT(_dataHierarchyItem != nullptr);

        _dataHierarchyItem->getDataset<Clusters>().getClusters() = _clustersModel.getClusters();

        _core->notifyDataChanged(_dataHierarchyItem->getDatasetName());
    });

    // Clear the selection when the layout of the model changes
    connect(&_clustersModel, &QAbstractItemModel::layoutChanged, this, [this](const QList<QPersistentModelIndex> &parents = QList<QPersistentModelIndex>(), QAbstractItemModel::LayoutChangeHint hint = QAbstractItemModel::NoLayoutChangeHint) {
        Q_ASSERT(_dataHierarchyItem != nullptr);

        _dataHierarchyItem->getDataset<Clusters>().getClusters() = _clustersModel.getClusters();

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
    _clustersModel.removeClustersById(ids);
}

ClustersModel& ClustersAction::getClustersModel()
{
    return _clustersModel;
}

ClustersAction::Widget::Widget(QWidget* parent, ClustersAction* clustersAction, const hdps::gui::WidgetActionWidget::State& state) :
    WidgetActionWidget(parent, clustersAction, state),
    _nameFilterAction(this, "Name filter"),
    _removeAction(this, "Remove"),
    _cacheClusterSelection(),
    _clustersFilterModel(this)
{
    _nameFilterAction.setPlaceHolderString("Filter by cluster name...");
    _removeAction.setEnabled(false);

    _clustersFilterModel.setSourceModel(&clustersAction->getClustersModel());
    _clustersFilterModel.setDynamicSortFilter(true);
    _clustersFilterModel.setFilterKeyColumn(static_cast<std::int32_t>(ClustersModel::Column::Name));

    auto clustersTreeView = new QTreeView();

    // Configure tree view
    clustersTreeView->setFixedHeight(150);
    clustersTreeView->setRootIsDecorated(false);
    clustersTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    clustersTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    clustersTreeView->setSortingEnabled(true);
    clustersTreeView->sortByColumn(static_cast<std::int32_t>(ClustersModel::Column::NumberOfIndices));
    clustersTreeView->setModel(&_clustersFilterModel);

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

    auto mainLayout = new QVBoxLayout();

    mainLayout->setMargin(0);
    mainLayout->addWidget(clustersTreeView);
    mainLayout->addWidget(_nameFilterAction.createWidget(this));

    setLayout(mainLayout);

    const auto selectionChangedHandler = [this, clustersAction, clustersTreeView]() -> void {

        // Point indices that need to be selected
        std::vector<std::uint32_t> selectionIndices;

        const auto selectedRows = clustersTreeView->selectionModel()->selectedRows();

        // Gather point indices for selection
        for (auto selectedIndex : selectedRows) {
            auto cluster = static_cast<Cluster*>(_clustersFilterModel.mapToSource(selectedIndex).internalPointer());
            selectionIndices.insert(selectionIndices.end(), cluster->_indices.begin(), cluster->_indices.end());
        }

        // Remove duplicates
        std::sort(selectionIndices.begin(), selectionIndices.end());
        selectionIndices.erase(unique(selectionIndices.begin(), selectionIndices.end()), selectionIndices.end());

        // Select points
        clustersAction->selectPoints(selectionIndices);

        // Update state of the remove action
        _removeAction.setEnabled(!selectedRows.isEmpty());
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
        const auto colorIndex   = _clustersFilterModel.mapToSource(index).siblingAtColumn(colorColumn);
        const auto currentColor = colorIndex.data(Qt::EditRole).value<QColor>();
        
        // Update the clusters model with the new color
        clustersAction->getClustersModel().setData(colorIndex, QColorDialog::getColor(currentColor));
    });

    auto toolbarLayout = new QHBoxLayout();

    mainLayout->addLayout(toolbarLayout);

    toolbarLayout->addWidget(_removeAction.createWidget(this));

    connect(&_removeAction, &TriggerAction::triggered, this, [this, clustersAction, clustersTreeView]() {
        const auto selectedRows = clustersTreeView->selectionModel()->selectedRows();

        QStringList clusterIds;

        for (auto selectedIndex : selectedRows)
            clusterIds << _clustersFilterModel.mapToSource(selectedIndex).siblingAtColumn(static_cast<std::int32_t>(ClustersModel::Column::ID)).data(Qt::DisplayRole).toString();

        clustersAction->removeClustersById(clusterIds);
    });

    const auto updateNameFilter = [this]() -> void {
        _clustersFilterModel.setNameFilter(_nameFilterAction.getString());
    };

    connect(&_nameFilterAction, &StringAction::stringChanged, this, [this, updateNameFilter](const QString& string) {
        updateNameFilter();
    });

    updateNameFilter();
}
