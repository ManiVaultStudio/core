#include "RemoveClustersAction.h"
#include "ClustersAction.h"
#include "ClustersFilterModel.h"
#include "ClusterData.h"

#include <QItemSelectionModel>

RemoveClustersAction::RemoveClustersAction(ClustersAction& clustersAction, ClustersFilterModel& filterModel, QItemSelectionModel& selectionModel) :
    TriggerAction(&clustersAction),
    _clustersAction(clustersAction),
    _filterModel(filterModel),
    _selectionModel(selectionModel)
{
    setText("Remove");
    setToolTip("Remove selected clusters");
    setIcon(Application::getIconFont("FontAwesome").getIcon("trash"));

    // Remove selected clusters when the action is triggered
    connect(this, &TriggerAction::triggered, this, [this]() {

        // Get the selected rows from the selection model
        const auto selectedRows = _selectionModel.selectedRows();

        QStringList clusterIds;

        // Build a list of cluster identifiers to remove
        for (auto selectedIndex : selectedRows)
            clusterIds << _filterModel.mapToSource(selectedIndex).siblingAtColumn(static_cast<std::int32_t>(ClustersModel::Column::ID)).data(Qt::DisplayRole).toString();

        // Remove the clusters
        _clustersAction.removeClustersById(clusterIds);
    });

    // Update action read only status
    const auto updateReadOnly = [this]() -> void {
        setEnabled(_selectionModel.selectedRows().count() >= 1);
    };

    // Update action read only status when the item selection changes
    connect(&_selectionModel, &QItemSelectionModel::selectionChanged, updateReadOnly);

    // Initialize read only status
    updateReadOnly();
}