// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ClustersActionWidget.h"
#include "ClustersAction.h"
#include "ClusterData.h"

#include <PointData/PointData.h>

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
    _clustersTreeView()
{
    // Configure filter model
    _filterModel.setSourceModel(&clustersAction->getClustersModel());
    _filterModel.setDynamicSortFilter(true);
    _filterModel.setFilterKeyColumn(static_cast<std::int32_t>(ClustersModel::Column::Name));

    // Configure tree view
    _clustersTreeView.setFocusPolicy(Qt::ClickFocus);
    _clustersTreeView.setModel(&_filterModel);
    _clustersTreeView.setSelectionModel(&_selectionModel);
    _clustersTreeView.setFixedHeight(300);
    _clustersTreeView.setRootIsDecorated(false);
    _clustersTreeView.setSelectionBehavior(QAbstractItemView::SelectRows);
    _clustersTreeView.setSelectionMode(QAbstractItemView::ExtendedSelection);
    _clustersTreeView.setSortingEnabled(false);
    _clustersTreeView.setUniformRowHeights(true);

    auto header = _clustersTreeView.header();
    
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

    // Clear the selection when the layout of the model changes
    connect(_clustersTreeView.model(), &QAbstractItemModel::layoutChanged, this, [this](const QList<QPersistentModelIndex> &parents = QList<QPersistentModelIndex>(), QAbstractItemModel::LayoutChangeHint hint = QAbstractItemModel::NoLayoutChangeHint) {
        _clustersTreeView.selectionModel()->reset();

        if (_clustersAction.getClustersModel().rowCount() < 100000) {
            _clustersTreeView.setSortingEnabled(true);
            _clustersTreeView.sortByColumn(static_cast<std::int32_t>(ClustersModel::Column::Name), Qt::SortOrder::AscendingOrder);
        }
    });

    // Pick cluster color when the cluster item is double clicked
    connect(&_clustersTreeView, &QTreeView::doubleClicked, this, [this](const QModelIndex& index) {
        const auto colorColumn = static_cast<std::int32_t>(ClustersModel::Column::Color);

        if (index.column() != colorColumn)
            return;

        // Get sibling color model index and the current cluster color
        const auto colorIndex   = _filterModel.mapToSource(index).siblingAtColumn(colorColumn);
        const auto currentColor = colorIndex.data(Qt::EditRole).value<QColor>();
        
        // Update the clusters model with the new color
        _clustersAction.getClustersModel().setData(colorIndex, QColorDialog::getColor(currentColor));
    });

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    // Add clusters tree view
    layout->addWidget(&_clustersTreeView);

    // Add remove/merge widgets if required
    if (widgetFlags & ClustersAction::Remove || widgetFlags & ClustersAction::Merge) {
        auto toolbarLayout = new QHBoxLayout();

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

        // Add toolbar to main layout
        layout->addLayout(toolbarLayout);
    }

    // Apply the layout
    setLayout(layout);

    // Perform an initial setup of the selection synchronization
    setupSelectionSynchronization();

    // Setup of the selection synchronization when the clusters dataset changes
    connect(&_clustersAction.getClustersDataset(), &Dataset<Clusters>::changed, this, &ClustersActionWidget::setupSelectionSynchronization);
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

void ClustersActionWidget::setupSelectionSynchronization()
{
    // Only setup selection synchronization when there is a valid clusters dataset
    if (!_clustersAction.getClustersDataset().isValid())
        return;

    // Remove previous connections
    disconnect(_clustersTreeView.selectionModel(), &QItemSelectionModel::selectionChanged, this, nullptr);
    disconnect(&_clustersAction.getClustersDataset(), &Dataset<Clusters>::dataSelectionChanged, this, nullptr);

    // Select cluster points when the cluster selection changes
    connect(_clustersTreeView.selectionModel(), &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection& selected, const QItemSelection& deselected) -> void {

        // Get selected row
        const auto selectedRows = _clustersTreeView.selectionModel()->selectedRows();

        // Indices of the selected clusters
        std::vector<std::uint32_t> selectedClustersIndices;

        // Reserve the selection indices
        selectedClustersIndices.reserve(_clustersAction.getClusters()->size());

        // Gather point indices for selection
        for (const auto& selectedIndex : selectedRows)
            selectedClustersIndices.push_back(_filterModel.mapToSource(selectedIndex).row());

        // Select clusters
        _clustersAction.getClustersDataset()->setSelectionIndices(selectedClustersIndices);
    });

    // Highlight selected clusters (selection made elsewhere)
    connect(&_clustersAction.getClustersDataset(), &Dataset<Clusters>::dataSelectionChanged, this, [this]() -> void {

        // Get selected rows
        auto& clusterData = _clustersAction.getClustersDataset();
        const auto& selectedClusters = clusterData->getSelectionIndices();

        QModelIndexList selectedRows = _selectionModel.selectedRows();
        if (selectedRows.size() == selectedClusters.size()) {
            bool alreadySelected = true;

            for (size_t i = 0; i < selectedClusters.size(); ++i) {
                if (selectedRows[i].row() != static_cast<int>(selectedClusters[i])) {
                    alreadySelected = false;
                    break;
                }
            }

            if (alreadySelected)
                return;
        }

        QItemSelection selection;

        for (const auto& row : selectedClusters) {
            const auto firstItemIndex   = _filterModel.index(static_cast<int>(row), 0);
            const auto lastItem         = _filterModel.index(static_cast<int>(row), 0);

            selection.select(firstItemIndex, lastItem);
        }

        QApplication::setOverrideCursor(Qt::WaitCursor);
        {
            // Deselect all rows
            const auto firstItemIndex   = _filterModel.index(0, 0);
            const auto lastItem         = _filterModel.index(_filterModel.rowCount() - 1, 0);

            _selectionModel.select(QItemSelection(firstItemIndex, lastItem), QItemSelectionModel::Rows | QItemSelectionModel::Deselect);

            // Select rows
            _selectionModel.select(selection, QItemSelectionModel::Rows | QItemSelectionModel::Select);
        }
        QApplication::restoreOverrideCursor();

    });
}
