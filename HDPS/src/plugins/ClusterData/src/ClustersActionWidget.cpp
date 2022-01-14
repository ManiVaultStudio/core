#include "ClustersActionWidget.h"
#include "ClustersAction.h"
#include "ClusterData.h"

#include <PointData.h>

#include <QTreeView>
#include <QHeaderView>
#include <QVBoxLayout>

ClustersActionWidget::ClustersActionWidget(QWidget* parent, ClustersAction* clustersAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, clustersAction),
    _filterModel(),
    _clustersAction(*clustersAction),
    _selectionModel(&_filterModel),
    _removeClustersAction(this),
    _mergeClustersAction(this),
    _filterClustersAction(this),
    _selectClustersAction(this),
    _subsetAction(this),
    _refreshClustersAction(this),
    _importClustersAction(this),
    _exportClustersAction(this)
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
    
    // Configure basic column sizing parameters
    header->setStretchLastSection(false);
    header->hideSection(static_cast<std::int32_t>(ClustersModel::Column::ID));
    header->hideSection(static_cast<std::int32_t>(ClustersModel::Column::ModifiedByUser));
    header->setMinimumSectionSize(20);

    // Set initial column widths
    header->resizeSection(static_cast<std::int32_t>(ClustersModel::Column::Color), 20);
    header->resizeSection(static_cast<std::int32_t>(ClustersModel::Column::Name), 200);
    header->resizeSection(static_cast<std::int32_t>(ClustersModel::Column::NumberOfIndices), 100);

    // Set column resize modes
    header->setSectionResizeMode(static_cast<std::int32_t>(ClustersModel::Column::Color), QHeaderView::Fixed);
    header->setSectionResizeMode(static_cast<std::int32_t>(ClustersModel::Column::Name), QHeaderView::Stretch);
    header->setSectionResizeMode(static_cast<std::int32_t>(ClustersModel::Column::NumberOfIndices), QHeaderView::Fixed);

    // Selection synchronization when there is a valid clusters dataset
    if (clustersAction->getClustersDataset().isValid()) {

        // Select cluster points when the cluster selection changes
        connect(clustersTreeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, [this, clustersAction, clustersTreeView](const QItemSelection &selected, const QItemSelection &deselected) -> void {

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
        });
    }

    auto points = clustersAction->getPointsDataset();

    if (points.isValid()) {

        // Select cluster points when the cluster selection changes
        connect(clustersTreeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, [this, points, clustersAction, clustersTreeView](const QItemSelection &selected, const QItemSelection &deselected) -> void {
            /*
            // Get selected row
            const auto selectedRows = clustersTreeView->selectionModel()->selectedRows();

            // Indices of the selected clusters
            std::vector<std::uint32_t> currentClusterSelectionIndices;

            // Clear and reserve the selection indices
            currentClusterSelectionIndices.reserve(clustersAction->getClusters()->size());

            auto selection              = points->getSelection<Points>();
            auto& pointSelectionIndices = selection->indices;

            pointSelectionIndices.clear();
            pointSelectionIndices.reserve(selectedRows.count());

            std::vector<std::uint32_t> globalIndices;

            points->getGlobalIndices(globalIndices);

            // Append point indices per cluster
            for (auto selectedIndex : selectedRows) {
                auto cluster = static_cast<Cluster*>(_filterModel.mapToSource(selectedIndex).internalPointer());

                // Append the indices
                pointSelectionIndices.insert(pointSelectionIndices.end(), cluster->getIndices().begin(), cluster->getIndices().end());
            }

            // Remove duplicates
            std::sort(pointSelectionIndices.begin(), pointSelectionIndices.end());
            pointSelectionIndices.erase(unique(pointSelectionIndices.begin(), pointSelectionIndices.end()), pointSelectionIndices.end());

            for (auto& pointSelectionIndex : pointSelectionIndices)
                pointSelectionIndex = globalIndices[pointSelectionIndex];

            // Notify others that the parent points selection has changed
            Application::core()->notifyDataSelectionChanged(points);
            */
        });
    }

    // Clear the selection when the layout of the model changes
    connect(clustersTreeView->model(), &QAbstractItemModel::layoutChanged, this, [this, clustersTreeView](const QList<QPersistentModelIndex> &parents = QList<QPersistentModelIndex>(), QAbstractItemModel::LayoutChangeHint hint = QAbstractItemModel::NoLayoutChangeHint) {
        clustersTreeView->selectionModel()->reset();
    });

    // Pick cluster color when the cluster item is double clicked
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

    auto layout = new QVBoxLayout();

    layout->setMargin(0);

    // Add clusters tree view
    layout->addWidget(clustersTreeView);

    // Add remove/merge widgets if required
    if (widgetFlags & ClustersAction::Remove || widgetFlags & ClustersAction::Merge) {
        auto toolbarLayout = new QHBoxLayout();

        toolbarLayout->setSpacing(3);

        // Add filter clusters widget if required
        if (widgetFlags & ClustersAction::Filter)
            toolbarLayout->addWidget(_filterClustersAction.createCollapsedWidget(this));

        // Add select cluster(s) widget if required
        if (widgetFlags & ClustersAction::Select)
            toolbarLayout->addWidget(_selectClustersAction.createCollapsedWidget(this));

        // Add remove cluster(s) widget
        if (widgetFlags & ClustersAction::Remove)
            toolbarLayout->addWidget(_removeClustersAction.createWidget(this, TriggerAction::Icon));

        // Add merge clusters widget
        if (widgetFlags & ClustersAction::Merge)
            toolbarLayout->addWidget(_mergeClustersAction.createWidget(this, TriggerAction::Icon));

        // Add colorize widget if required
        if (widgetFlags & ClustersAction::Colorize)
            toolbarLayout->addWidget(_clustersAction.getColorizeClustersAction().createCollapsedWidget(this));

        // Add prefix clusters widget if required
        if (widgetFlags & ClustersAction::Prefix)
            toolbarLayout->addWidget(_clustersAction.getPrefixClustersAction().createCollapsedWidget(this));

        // Add subset widget if required
        if (widgetFlags & ClustersAction::Subset)
            toolbarLayout->addWidget(_subsetAction.createCollapsedWidget(this));

        // Add refresh widget if required
        if (widgetFlags & ClustersAction::Refresh)
            toolbarLayout->addWidget(_refreshClustersAction.createWidget(this, TriggerAction::Icon));

        toolbarLayout->addStretch(1);

        // Add import clusters widget if required
        if (widgetFlags & ClustersAction::Import)
            toolbarLayout->addWidget(_importClustersAction.createWidget(this, TriggerAction::Icon));

        // Add export clusters widget if required
        if (widgetFlags & ClustersAction::Export)
            toolbarLayout->addWidget(_exportClustersAction.createWidget(this, TriggerAction::Icon));

        // Add toolbar to main layout
        layout->addLayout(toolbarLayout);
    }

    // Apply the layout
    setLayout(layout);
}

ClustersAction& ClustersActionWidget::getClustersAction()
{
    return _clustersAction;
}

ClustersFilterModel& ClustersActionWidget::getFilterModel()
{
    return _filterModel;
}

QItemSelectionModel& ClustersActionWidget::getSelectionModel()
{
    return _selectionModel;
}
