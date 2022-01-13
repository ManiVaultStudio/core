#include "MergeClustersAction.h"
#include "ClustersAction.h"
#include "ClustersFilterModel.h"
#include "ClusterData.h"

#include <QItemSelectionModel>

MergeClustersAction::MergeClustersAction(ClustersAction& clustersAction, ClustersFilterModel& filterModel, QItemSelectionModel& selectionModel) :
    TriggerAction(&clustersAction),
    _clustersAction(clustersAction),
    _filterModel(filterModel),
    _selectionModel(selectionModel)
{
    setText("");
    setToolTip("Merge selected clusters into one");
    setIcon(Application::getIconFont("FontAwesome").getIcon("object-group"));

    // Merge selected clusters when the action is triggered
    connect(this, &TriggerAction::triggered, this, [this]() {

        // Get the selected rows from the selection model
        const auto selectedRows = _selectionModel.selectedRows();

        // All cluster except the first one need to be removed after the merge process
        QStringList clusterIdsToRemove;

        // Determine merge cluster
        auto mergeCluster = static_cast<Cluster*>(_filterModel.mapToSource(selectedRows.first()).internalPointer());

        // Alter name of the merge cluster
        mergeCluster->setName(QString("%1*").arg(mergeCluster->getName()));

        // Move cluster indices of remaining clusters into the merge cluster
        for (auto selectedIndex : selectedRows) {

            // Get pointer to cluster
            auto cluster = static_cast<Cluster*>(_filterModel.mapToSource(selectedIndex).internalPointer());

            // Do not move merge cluster indices into itself
            if (selectedIndex == selectedRows.first())
                continue;

            // Flag the cluster identifier to be removed
            clusterIdsToRemove << cluster->getId();

            // Move the cluster indices into the merge cluster
            mergeCluster->getIndices().insert(mergeCluster->getIndices().end(), cluster->getIndices().begin(), cluster->getIndices().end());
        }

        // Remove the redundant clusters
        _clustersAction.removeClustersById(clusterIdsToRemove);
    });

    // Update action read only status
    const auto updateReadOnly = [this]() -> void {
        setEnabled(_selectionModel.selectedRows().count() >= 2);
    };

    // Update action read only status when the item selection changes
    connect(&_selectionModel, &QItemSelectionModel::selectionChanged, updateReadOnly);
    connect(&_filterModel, &QAbstractItemModel::layoutChanged, this, updateReadOnly);

    // Initialize read only status
    updateReadOnly();
}