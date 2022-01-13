#include "ClustersAction.h"
#include "ClusterData.h"

#include <event/Event.h>

#include <QTreeView>
#include <QHeaderView>
#include <QVBoxLayout>

ClustersAction::ClustersAction(QObject* parent, Dataset<Clusters> clustersDataset, Dataset<Points> pointsDataset /*= Dataset<Points>()*/) :
    WidgetAction(parent),
    _clustersDataset(clustersDataset),
    _pointsDataset(pointsDataset),
    _clustersModel(),
    _colorizeClustersAction(*this)
{
    setText("Clusters");
    setDefaultWidgetFlags(WidgetFlag::Default);

    // Automatically synchronize clusters when we have a valid clusters dataset
    if (_clustersDataset.isValid()) {

        // Update the clusters model to reflect the changes in the clusters set
        connect(&_clustersDataset, &Dataset<Clusters>::dataChanged, this, [this]() {
            _clustersModel.setClusters(*getClusters());
        });

        // Update the clusters in the clusters dataset
        const auto updateClusters = [this]() -> void {

            // transfer clusters to the clusters dataset
            _clustersDataset->getClusters() = _clustersModel.getClusters();

            // Notify others that the clusters changed
            Application::core()->notifyDataChanged(_clustersDataset);
        };

        // Update the clusters dataset when the model data and/or layout changes
        connect(&_clustersModel, &QAbstractItemModel::dataChanged, this, updateClusters);
        connect(&_clustersModel, &QAbstractItemModel::layoutChanged, this, updateClusters);
    }
}

QVector<Cluster>* ClustersAction::getClusters()
{
    if (!_clustersDataset.isValid())
        return nullptr;

    return &_clustersDataset->getClusters();
}

Dataset<Clusters>& ClustersAction::getClustersDataset()
{
    return _clustersDataset;
}

Dataset<Points>& ClustersAction::getPointsDataset()
{
    return _pointsDataset;
}

void ClustersAction::createSubset(const QString& datasetName)
{
    _clustersDataset->getParent()->createSubset("Clusters", _clustersDataset->getParent());
}

void ClustersAction::removeClustersById(const QStringList& ids)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    {
        _clustersModel.removeClustersById(ids);
    }
    QApplication::restoreOverrideCursor();
}

ClustersModel& ClustersAction::getClustersModel()
{
    return _clustersModel;
}

void ClustersAction::invalidateClusters()
{
    emit refreshClusters();
}
