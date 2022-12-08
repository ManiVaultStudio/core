#include "PluginManagerModel.h"

#include <Application.h>
#include <AbstractPluginManager.h>

#include <util/Exception.h>

using namespace hdps;
using namespace hdps::util;

#ifdef _DEBUG
    #define PLUGIN_MANAGER_MODEL_VERBOSE
#endif

PluginManagerModel::PluginManagerModel(QObject* parent /*= nullptr*/) :
    QAbstractItemModel(parent)
{
}

int PluginManagerModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
    return 0;
}

int PluginManagerModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
    return static_cast<int>(Column::Count);
}

QModelIndex PluginManagerModel::index(int row, int column, const QModelIndex& parent /*= QModelIndex()*/) const
{
    return createIndex(row, column, static_cast<void*>(nullptr));
}

QModelIndex PluginManagerModel::parent(const QModelIndex& index) const
{
    return QModelIndex();
}

QVariant PluginManagerModel::data(const QModelIndex& index, int role) const
{
    const auto row      = index.row();
    const auto column   = index.column();

    switch (role)
    {
        case Qt::EditRole:
        {
            switch (column)
            {
                //case Column::Name:
                //    return action->text();

                //case Column::Type:
                //    return action->getTypeString();

                //case Column::IsPublic:
                //    return action->isPublic();

                //case Column::NumberOfConnections:
                //    return QVariant::fromValue(_publicActions[row]->getConnectedActions().count());

                default:
                    break;
            }

            break;
        }

        case Qt::DisplayRole:
        {
            switch (column)
            {
                //case Column::Name:
                //    return data(index, Qt::EditRole).toString();

                //case Column::Type:
                //    return data(index, Qt::EditRole).toString();

                //case Column::IsPublic:
                //    return data(index, Qt::EditRole).toBool() ? "true" : "false";

                //case Column::NumberOfConnections:
                //    return QString::number(data(index, Qt::EditRole).toInt());

                default:
                    break;
            }

            break;
        }

        case Qt::ToolTipRole:
        {
            switch (column)
            {
                //case Column::Name:
                //    return data(index, Qt::EditRole).toString();

                //case Column::Type:
                //    return data(index, Qt::EditRole).toString();

                //case Column::IsPublic:
                //    return "Parameter is" + QString(data(index, Qt::EditRole).toBool() ? "" : "not") + " public";

                //case Column::NumberOfConnections:
                //{
                //    QStringList connectedActionNames;

                //    for (auto connectedAction : action->getConnectedActions())
                //        connectedActionNames << connectedAction->getSettingsPath();

                //    return QVariant::fromValue(connectedActionNames.join("\n"));
                //}

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

bool PluginManagerModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
{
    //const auto row      = index.row();
    //const auto column   = index.column();
    //const auto action   = static_cast<WidgetAction*>(index.internalPointer());

    //switch (role) {
    //    case Qt::EditRole:
    //    {
    //        switch (column)
    //        {
    //            case Column::Name:
    //                action->setText(value.toString());
    //                break;

    //            case Column::Type:
    //            case Column::IsPublic:
    //            case Column::NumberOfConnections:
    //                break;

    //            default:
    //                break;
    //        }

    //        break;
    //    }

    //    default:
    //        break;
    //}

    //emit dataChanged(index, index);

    return true;
}

QVariant PluginManagerModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
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

                    case Column::Type:
                        return "Type";

                    case Column::Kind:
                        return "Kind";

                    case Column::ID:
                        return "ID";

                    case Column::NumberOfInstances:
                        return "# Instances";

                    default:
                        break;
                }

                break;
            }

            case Qt::ToolTipRole:
            {
                switch (static_cast<Column>(section))
                {
                    case Column::Name:
                        return "Name of the plugin (instance)";

                    case Column::Type:
                        return "Type of plugin (e.g. loader/writer/analysis)";

                    case Column::Kind:
                        return "Kind of plugin";

                    case Column::ID:
                        return "Globally unique identifier of the plugin instance";

                    case Column::NumberOfInstances:
                        return "Number of instances of the plugin kind";

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

Qt::ItemFlags PluginManagerModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    auto itemFlags = Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | QAbstractItemModel::flags(index);

    return itemFlags;
}

void PluginManagerModel::initializeFromPluginManager()
{
#ifdef PLUGIN_MANAGER_MODEL_VERBOSE
    qDebug() << __FUNCTION__;
#endif
}
