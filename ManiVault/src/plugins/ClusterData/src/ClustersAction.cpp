// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ClustersAction.h"
#include "ClusterData.h"

#include <event/Event.h>

#include <QTreeView>
#include <QHeaderView>
#include <QVBoxLayout>

ClustersAction::ClustersAction(QObject* parent, Dataset<Clusters> clustersDataset /*= Dataset<Clusters>()*/) :
    WidgetAction(parent, "Clusters"),
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
    if (_clustersDataset.isValid()) {
        disconnect(&_clustersDataset, &Dataset<Clusters>::dataChanged, this, nullptr);
        disconnect(&_clustersModel, &QAbstractItemModel::dataChanged, this, nullptr);
        disconnect(&_clustersModel, &QAbstractItemModel::layoutChanged, this, nullptr);
    }

    _clustersDataset = clustersDataset;

    // Automatically synchronize clusters when we have a valid clusters dataset
    if (_clustersDataset.isValid()) {

        // Update the clusters model to reflect the changes in the clusters dataset
        connect(&_clustersDataset, &Dataset<Clusters>::dataChanged, this, &ClustersAction::updateClustersModel);

        // Update the clusters dataset when the model data and/or layout changes
        connect(&_clustersModel, &QAbstractItemModel::dataChanged, this, &ClustersAction::updateClustersDataset);
        connect(&_clustersModel, &QAbstractItemModel::layoutChanged, this, &ClustersAction::updateClustersDataset);
    }
}

void ClustersAction::createSubsetFromSelection(const QString& datasetName)
{
    _clustersDataset->getParent()->createSubsetFromSelection(datasetName, _clustersDataset->getParent());
}

void ClustersAction::removeClustersById(const QStringList& ids)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    {
        _clustersModel.removeClustersById(ids);
        updateClustersDataset();
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
    auto clusters = getClusters();

    if (clusters)
        _clustersModel.setClusters(*clusters);
}

void ClustersAction::updateClustersDataset()
{
    const auto& newClusters     = _clustersModel.getClusters();
    const auto& currentClusters = _clustersDataset->getClusters();

    if (newClusters == currentClusters)
        return;

    bool automaticRecolor = newClusters.size() != currentClusters.size();

    _clustersDataset->setClusters(newClusters);

    // Re-color if a cluster was deleted
    if (automaticRecolor)
        _colorizeClustersAction.updateColorsInModel();

    events().notifyDatasetDataChanged(_clustersDataset);
}
