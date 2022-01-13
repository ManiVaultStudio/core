#include "ClustersModel.h"
#include "ClusterData.h"

#include <QPainter>

using namespace hdps;

ClustersModel::ClustersModel(QObject* parent /*= nullptr*/) :
    QAbstractListModel(parent),
    _clusters()
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

    switch (role)
    {
        case Qt::EditRole:
        {
            switch (column) {
                case Column::Color:
                    cluster->setColor(value.value<QColor>());
                    break;

                case Column::Name:
                    cluster->setName(value.toString());
                    break;

                case Column::ID:
                case Column::NumberOfIndices:
                    break;

                default:
                    break;
            }
        }

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
                    case Column::Color:
                        return "";

                    case Column::Name:
                        return "Name";

                    case Column::ID:
                        return "Identifier";

                    case Column::NumberOfIndices:
                        return "# Points";

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
                        return Application::getIconFont("FontAwesome").getIcon("palette", QSize(12, 12));

                    case Column::Name:
                    case Column::ID:
                    case Column::NumberOfIndices:
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

void ClustersModel::setClusters(const QVector<Cluster>& clusters)
{
    const auto numberOfClustersChanged = clusters.size() != _clusters.size();

    if (numberOfClustersChanged) {

        emit layoutAboutToBeChanged();
        {
            _clusters = clusters;
        }
        emit layoutChanged();
    }
    else {
        if (!std::equal(_clusters.begin(), _clusters.end(), clusters.begin())) {
            _clusters = clusters;

            emit dataChanged(index(0, 0), index(rowCount() - 1, static_cast<std::int32_t>(Column::Count) - 1));
        }
    }
}

void ClustersModel::removeClustersById(const QStringList& ids)
{
    emit layoutAboutToBeChanged();
    {
        for (auto id : ids) {
            _clusters.erase(std::remove_if(_clusters.begin(), _clusters.end(), [id](const Cluster& cluster) -> bool
            {
                return cluster.getId() == id;
            }), _clusters.end());
        }
    }
    emit layoutChanged();
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
