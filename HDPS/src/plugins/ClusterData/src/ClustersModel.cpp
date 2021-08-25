#include "ClustersModel.h"
#include "ClusterData.h"

#include <QPainter>

using namespace hdps;

ClustersModel::ClustersModel(QObject* parent, std::vector<Cluster>* clusters) :
    QAbstractListModel(parent),
    _clusters(clusters)
{
    Q_ASSERT(_clusters != nullptr);
}

int ClustersModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
    Q_ASSERT(_clusters != nullptr);
    
    return static_cast<std::int32_t>(_clusters->size());
}

int ClustersModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
    Q_ASSERT(_clusters != nullptr);

    return static_cast<std::int32_t>(Column::_End);
}

QVariant ClustersModel::data(const QModelIndex& index, int role) const
{
    Q_ASSERT(_clusters != nullptr);

    auto& cluster = _clusters->at(index.row());

    const auto column = static_cast<Column>(index.column());

    switch (role)
    {
        case Qt::DecorationRole:
            return column == Column::Name ? getDecorationRole(cluster._color) : QVariant();

        case Qt::DisplayRole:
        {
            switch (column)
            {
                case Column::Name:
                    return cluster._name;

                case Column::NumberOfPoints:
                    return cluster.indices.size();
            }

            break;
        }

        case Qt::EditRole:
        {
            switch (column)
            {
                case Column::Name:
                    return cluster._name;

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

    auto cluster = static_cast<Cluster*>((void*)index.internalPointer());

    switch (column) {
        case Column::Name:
            cluster->_name = value.toString();
            break;

        default:
            break;
    }

    emit dataChanged(index, index);

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
                    case Column::Name:
                        return "Name";

                    case Column::NumberOfPoints:
                        return "# Points";

                    default:
                        return "";
                }
            }
        }
    }

    return QVariant();
}

QModelIndex ClustersModel::index(int row, int column, const QModelIndex& parent /*= QModelIndex()*/) const
{
    return createIndex(row, column, &_clusters->at(row));
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

Cluster& ClustersModel::getCluster(const std::int32_t& row)
{
    return _clusters->at(row);
}

QIcon ClustersModel::getDecorationRole(const QColor& color) const
{
    QPixmap pixmap(QSize(14, 14));

    pixmap.fill(color);

    QPainter painter(&pixmap);

    return QIcon(pixmap);
}
