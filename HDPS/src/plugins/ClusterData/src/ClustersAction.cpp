#include "ClustersAction.h"
#include "ClusterData.h"

#include <event/Event.h>

#include <QTreeView>
#include <QHeaderView>
#include <QVBoxLayout>

ClustersAction::ClustersAction(QObject* parent, Dataset<Clusters> clustersDataset /*= Dataset<Clusters>()*/) :
    WidgetAction(parent),
    _clustersDataset(),
    _clustersModel(),
    _colorizeClustersAction(*this),
    _prefixClustersAction(*this)
{
    setText("Clusters");
    setDefaultWidgetFlags(WidgetFlag::Default);
    setClustersDataset(clustersDataset);
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

void ClustersAction::setClustersDataset(Dataset<Clusters> clustersDataset)
{
    _clustersDataset = clustersDataset;

    // Automatically synchronize clusters when we have a valid clusters dataset
    if (_clustersDataset.isValid()) {

        // Update the clusters model to reflect the changes in the clusters dataset
        connect(&_clustersDataset, &Dataset<Clusters>::dataChanged, this, &ClustersAction::updateClustersModel);

        // Perform an initial update of the clusters model to reflect the state of the clusters dataset
        updateClustersModel();

        // Update the clusters dataset when the model data and/or layout changes
        connect(&_clustersModel, &QAbstractItemModel::dataChanged, this, &ClustersAction::updateClustersDataset);
        connect(&_clustersModel, &QAbstractItemModel::layoutChanged, this, &ClustersAction::updateClustersDataset);
    }
}

void ClustersAction::createSubset(const QString& datasetName)
{
    _clustersDataset->getParent()->createSubset(datasetName, _clustersDataset->getParent());
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

void ClustersAction::updateClustersModel()
{
    _clustersModel.setClusters(*getClusters());
}

void ClustersAction::updateClustersDataset()
{
    // Only update if the clusters have changed
    if (_clustersModel.getClusters() == _clustersDataset->getClusters())
        return;

    // Transfer clusters to the clusters dataset
    _clustersDataset->getClusters() = _clustersModel.getClusters();

    // Notify others that the clusters changed
    Application::core()->notifyDatasetChanged(_clustersDataset);
}
