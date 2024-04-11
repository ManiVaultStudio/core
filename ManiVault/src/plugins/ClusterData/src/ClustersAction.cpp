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
    qDebug() << "ClustersAction::updateClustersDataset START";

    qDebug() << "_clustersModel.getClusters";
    qDebug() << _clustersModel.getClusters()[0].getColor();
    qDebug() << "_clustersDataset->getClusters";
    qDebug() << _clustersDataset->getClusters()[0].getColor();

    auto temp = _clustersModel.getClusters()[0].getColor();

    const auto& newClusters = _clustersModel.getClusters();
    const auto& currentClusters = _clustersDataset->getClusters();

    if (newClusters == currentClusters)
    {
        qDebug() << "ClustersAction::updateClustersDataset RETURN";
        return;
    }

    qDebug() << "ClustersAction::updateClustersDataset YES CHANGE";

    bool automaticRecolor = newClusters.size() != currentClusters.size();

    _clustersDataset->setClusters(newClusters);

    qDebug() << "ClustersAction::updateClustersDataset MIDDLE 1";

    qDebug() << "_clustersModel.getClusters";
    qDebug() << _clustersModel.getClusters()[0].getColor();
    qDebug() << "_clustersDataset->getClusters";
    qDebug() << _clustersDataset->getClusters()[0].getColor();

    if (automaticRecolor)
        _colorizeClustersAction.updateColorsInModel();

    qDebug() << "ClustersAction::updateClustersDataset MIDDLE 2";

    qDebug() << "_clustersModel.getClusters";
    qDebug() << _clustersModel.getClusters()[0].getColor();
    qDebug() << "_clustersDataset->getClusters";
    qDebug() << _clustersDataset->getClusters()[0].getColor();

    events().notifyDatasetDataChanged(_clustersDataset);

    qDebug() << "ClustersAction::updateClustersDataset MIDDLE 3";

    qDebug() << "_clustersModel.getClusters";
    qDebug() << _clustersModel.getClusters()[0].getColor();
    qDebug() << "_clustersDataset->getClusters";
    qDebug() << _clustersDataset->getClusters()[0].getColor();

    qDebug() << "ClustersAction::updateClustersDataset END";
}
