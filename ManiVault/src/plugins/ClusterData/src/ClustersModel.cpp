// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ClustersModel.h"
#include "ClusterData.h"
#include "OverwriteClustersConfirmationDialog.h"

#include <QPainter>
#include <QEventLoop>

using namespace mv;
using namespace mv::util;

ClustersModel::ClustersModel(QObject* parent /*= nullptr*/) :
    QAbstractListModel(parent),
    _clusters(),
    _modifiedByUser(),
    _clusterPrefix()
{
}

int ClustersModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
    return static_cast<std::int32_t>(_clusters.size());
}

int ClustersModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
    return static_cast<std::int32_t>(Column::Count);
}

QVariant ClustersModel::data(const QModelIndex& index, int role) const
{
    const auto& cluster = _clusters.at(index.row());

    const auto column = static_cast<Column>(index.column());

    switch (role)
    {
        //case Qt::TextAlignmentRole:
            //return column == Column::NumberOfIndices ? Qt::AlignRight : QVariant();

        case Qt::DecorationRole:
            return column == Column::Color ? getColorIcon(cluster.getColor()) : QVariant();

        case Qt::ToolTipRole:
        {
            switch (column)
            {
                case Column::Color:
                    return QString("Color: %1").arg(data(index, Qt::DisplayRole).toString());

                case Column::Name:
                    return QString("Name: %1").arg(data(index, Qt::DisplayRole).toString());

                case Column::ID:
                    return QString("Identifier: %1").arg(data(index, Qt::DisplayRole).toString());

                case Column::NumberOfIndices:
                {
                    QStringList indices;

                    for (auto index : cluster.getIndices())
                        indices << QString::number(index);

                    return QString("Indices: [%1]").arg(indices.join(", "));
                }

                case Column::ModifiedByUser:
                    return QString().arg("Cluster has been modified manually") + (data(index, Qt::DisplayRole).toBool() ? "" : "not") + "";
            }

            break;
        }

        case Qt::DisplayRole:
        {
            switch (column)
            {
                case Column::Color:
                    return QString("rgb(%1, %2, %3").arg(QString::number(cluster.getColor().red()), QString::number(cluster.getColor().green()), QString::number(cluster.getColor().blue()));

                case Column::Name:
                    return cluster.getName();

                case Column::ID:
                    return cluster.getId();

                case Column::NumberOfIndices:
                    return QString::number(cluster.getNumberOfIndices());

                case Column::ModifiedByUser:
                    return data(index, Qt::EditRole).toBool() ? "true" : "false";
            }

            break;
        }

        case Qt::EditRole:
        {
            switch (column)
            {
                case Column::Color:
                    return QVariant::fromValue(cluster.getColor());

                case Column::Name:
                    return cluster.getName();

                case Column::ID:
                    return cluster.getId();

                case Column::NumberOfIndices:
                    return cluster.getNumberOfIndices();

                case Column::ModifiedByUser:
                    return _modifiedByUser[index.row()];

                default:
                    break;
            }

            break;
        }

        default:
            break;
    }

    return QVariant();
}

bool ClustersModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
{
    const auto column = static_cast<Column>(index.column());

    auto& cluster = _clusters[index.row()];

    switch (role)
    {
        case Qt::EditRole:
        {
            switch (column) {
                case Column::Color:
                    cluster.setColor(value.value<QColor>());
                    _modifiedByUser[index.row()] = true;
                    break;

                case Column::Name:
                    cluster.setName(value.toString());
                    _modifiedByUser[index.row()] = true;
                    break;

                case Column::ModifiedByUser:
                    _modifiedByUser[index.row()] = value.toBool();
                    break;

                case Column::ID:                [[fallthrough]];
                case Column::NumberOfIndices:   [[fallthrough]];

                default:
                    break;
            }

            break;
        }

        default:
            break;
    }

    emit dataChanged(index, index, { index.column() });

    return true;
}

QVariant ClustersModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    if (orientation == Qt::Horizontal) {
        switch (role)
        {
            case Qt::DisplayRole:
            {
                switch (static_cast<Column>(section))
                {
                    case Column::Color:
                        return "";

                    case Column::Name:
                        return "Name";

                    case Column::ID:
                        return "Identifier";

                    case Column::NumberOfIndices:
                        return "# Points";

                    case Column::ModifiedByUser:
                        return "Modified by user";

                    default:
                        break;
                }

                break;
            }

            case Qt::DecorationRole:
            {
                switch (static_cast<Column>(section))
                {
                    case Column::Color:
                        return StyledIcon("palette");

                    case Column::Name:
                    case Column::ID:
                    case Column::NumberOfIndices:
                    case Column::ModifiedByUser:
                        break;

                    default:
                        break;
                }

                break;
            }

            default:
                break;
        }
    }

    return QVariant();
}

QModelIndex ClustersModel::index(int row, int column, const QModelIndex& parent /*= QModelIndex()*/) const
{
    return createIndex(row, column, (void*)(&_clusters.at(row)));
}

Qt::ItemFlags ClustersModel::flags(const QModelIndex& index) const
{
    const auto column = static_cast<Column>(index.column());

    if (!index.isValid())
        return Qt::NoItemFlags;

    auto itemFlags = Qt::ItemIsSelectable | QAbstractItemModel::flags(index);

    switch (column)
    {
        case Column::Name:
            itemFlags |= Qt::ItemIsEditable;
            break;

        default:
            break;
    }

    return itemFlags;
}

QVector<Cluster>& ClustersModel::getClusters()
{
    return _clusters;
}

const QVector<Cluster>& ClustersModel::getClusters() const
{
    return _clusters;
}

void ClustersModel::setClusters(const QVector<Cluster>& clusters)
{
    // Allocate the same number of items for the modified by user column
    _modifiedByUser.resize(clusters.count());

    // And flag all clusters as unmodified
    std::fill(_modifiedByUser.begin(), _modifiedByUser.end(), false);

    emit layoutAboutToBeChanged();
    {
        // Assign clusters
        _clusters = clusters;
    }
    emit layoutChanged();

    assert(_clusters.size() == _modifiedByUser.size());

    /*
    const auto numberOfClustersChanged = clusters.size() != _clusters.size();

    //if (numberOfClustersChanged) {

        // Do nothing if user modified clusters may not be overridden
        //if (mayOverrideUserInput()) {

            // Allocate the same number of items for the modified by user column
            _modifiedByUser.resize(clusters.count());

            // And flag all clusters as unmodified
            std::fill(_modifiedByUser.begin(), _modifiedByUser.end(), false);

            emit layoutAboutToBeChanged();
            {
                // Assign clusters
                _clusters = clusters;
            }
            emit layoutChanged();
        //}
    //}
    //else {
    //    if (!std::equal(_clusters.begin(), _clusters.end(), clusters.begin())) {

    //        // Do nothing if user modified clusters may not be overridden
    //        //if (mayOverrideUserInput()) {

    //            // Assign clusters
    //            _clusters = clusters;

    //            // Notify others that the data has changed
    //            emit dataChanged(index(0, 0), index(rowCount() - 1, static_cast<std::int32_t>(Column::Count) - 1));
    //        //}
    //    }
    //}
    */
}

void ClustersModel::removeClustersById(const QStringList& ids)
{

    // Identify the indices that are to be removed from the holder vectors
    std::vector<qsizetype> rowsToBeRemoved;
    rowsToBeRemoved.reserve(ids.size());
    for (qsizetype i = 0; i < _clusters.size(); i++)
    {
        for (const auto& id : ids) {
            if (_clusters[i].getId() == id)
                rowsToBeRemoved.emplace_back(i);
        }
    }

    // Sort from larger to lower IDs, so that we erase from the back (to keep all revmove-IDs valid)
    std::sort(rowsToBeRemoved.begin(), rowsToBeRemoved.end(), std::greater<qsizetype>());
    assert(rowsToBeRemoved.back() < _clusters.size());

    // Remove entries from both the cluster and modifiedByUser holder 
    for (const auto& row : rowsToBeRemoved) {
        beginRemoveRows({}, row, row);

        _clusters.erase(_clusters.begin() + row);
        _modifiedByUser.erase(_modifiedByUser.begin() + row);

        endRemoveRows();
    }

    assert(_clusters.size() == _modifiedByUser.size());
}

void ClustersModel::setClusterPrefix(const QString& clusterPrefix)
{
    // Assign clusters prefix
    _clusterPrefix = clusterPrefix;

    // Apply the cluster prefix to all clusters
    applyClusterPrefixToAllClusters();
}

void ClustersModel::colorizeClusters(std::int32_t randomSeed /*= 0*/)
{
    if (_clusters.isEmpty())
        return;

    // Colorize clusters by pseudo-random colors
    Cluster::colorizeClusters(_clusters, randomSeed);

    // Notify others that the data for the color column has changed
    emit dataChanged(index(0, static_cast<std::int32_t>(Column::Color)), index(rowCount() - 1, static_cast<std::int32_t>(Column::Color)));
}

void ClustersModel::colorizeClusters(const QImage& colorMapImage)
{
    if (_clusters.isEmpty())
        return;

    // Colorize clusters by color map
    Cluster::colorizeClusters(_clusters, colorMapImage);

    // Notify others that the data for the color column has changed
    emit dataChanged(index(0, static_cast<std::int32_t>(Column::Color)), index(rowCount() - 1, static_cast<std::int32_t>(Column::Color)));
}

std::uint32_t ClustersModel::getNumberOfUserModifiedClusters() const
{
    return std::count(_modifiedByUser.begin(), _modifiedByUser.end(), true);
}

bool ClustersModel::hasUserModifications() const
{
    return getNumberOfUserModifiedClusters() >= 1;
}

bool ClustersModel::doAllClusterNamesStartWith(const QString& prefix)
{
    // No if there are no rows
    if (rowCount() == 0)
        return false;

    // Establish how many clusters start with the supplied prefix
    const auto numberOfMatches = match(index(0, static_cast<std::int32_t>(Column::Name)), Qt::EditRole, prefix, -1, Qt::MatchFlag::MatchStartsWith).count();

    // All clusters start with the same prefix if the number of matches equals the number of rows in the model
    return numberOfMatches == rowCount();
}

bool ClustersModel::mayOverrideUserInput()
{
    // Get the number of clusters that have been modified by the user
    const auto numberOfUserModifiedClusters = getNumberOfUserModifiedClusters();

    // No local modifications, so overwrite without user approval
    if (numberOfUserModifiedClusters == 0)
        return true;

    // Ask for confirmation dialog
    if (Application::current()->getSetting("OverwriteClustersAskConfirmation", true).toBool() == true) {

        // Create overwrite clusters dialog
        OverwriteClustersConfirmationDialog overwriteClustersConfirmationDialog(nullptr, _clusters.count(), numberOfUserModifiedClusters);

        // Show the confirm data removal dialog
        overwriteClustersConfirmationDialog.open();

        QEventLoop eventLoop;
        QObject::connect(&overwriteClustersConfirmationDialog, &QDialog::finished, &eventLoop, &QEventLoop::quit);
        eventLoop.exec();

        // If the user accepted, the clusters may be overwritten, otherwise not
        return overwriteClustersConfirmationDialog.result() == 1 ? true : false;
    }

    return true;
}

QIcon ClustersModel::getColorIcon(const QColor& color) const
{
    QPixmap pixmap(QSize(14, 14));

    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);

    const auto radius = 3;

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(color));
    painter.drawRoundedRect(0, 0, 14, 14, radius, radius);

    return QIcon(pixmap);
}

void ClustersModel::applyClusterPrefixToAllClusters()
{
    if (rowCount() == 0)
        return;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    {
        // Change the prefix of the clusters
        for (auto& cluster : _clusters)
            cluster.setName(_clusterPrefix + QString::number(_clusters.indexOf(cluster) + 1));

        // And flag all clusters as unmodified
        std::fill(_modifiedByUser.begin(), _modifiedByUser.end(), false);

        // Notify others that the data for the name column has changed
        emit dataChanged(index(0, static_cast<std::int32_t>(Column::Name)), index(rowCount() - 1, static_cast<std::int32_t>(Column::Name)));
    }
    QApplication::restoreOverrideCursor();
}
