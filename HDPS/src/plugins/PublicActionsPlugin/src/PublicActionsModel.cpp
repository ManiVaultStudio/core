#include "PublicActionsModel.h"

#include <util/Exception.h>

using namespace hdps::gui;
using namespace hdps::util;

int ActionsModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
    return _publicActions.count();
}

int ActionsModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
    return 4;
}

QModelIndex ActionsModel::index(int row, int column, const QModelIndex& parent /*= QModelIndex()*/) const
{
    return createIndex(row, column, static_cast<void*>(_publicActions.at(row)));
}

void ActionsModel::addPublicAction(WidgetAction* publicAction)
{
    try
    {
        // Insert the layer action at the beginning
        beginInsertRows(QModelIndex(), 0, 0);
        {
            _publicActions.insert(0, publicAction);

            const auto updateNumberOfConnectionsColumn = [this, publicAction]() -> void {
                const auto changedCell = index(_publicActions.indexOf(publicAction), Column::NumberOfConnections);
                emit dataChanged(changedCell, changedCell);
            };

            connect(publicAction, &WidgetAction::actionConnected, this, updateNumberOfConnectionsColumn);
            connect(publicAction, &WidgetAction::actionDisconnected, this, updateNumberOfConnectionsColumn);
        }
        endInsertRows();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to add action to the public actions model", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to add action to the public actions model");
    }
}

void ActionsModel::removePublicAction(WidgetAction* publicAction)
{
    try
    {
        if (!_publicActions.contains(publicAction))
            throw std::runtime_error("Action does not exist");

        const auto row = _publicActions.indexOf(publicAction);

        beginRemoveRows(QModelIndex(), row, row);
        {
            _publicActions.removeAt(row);

            delete publicAction;
        }
        endRemoveRows();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to remove action from the public actions model", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to remove action from the public actions model");
    }
}

QVariant ActionsModel::data(const QModelIndex& index, int role) const
{
    const auto row      = index.row();
    const auto column   = index.column();
    const auto action   = static_cast<WidgetAction*>(index.internalPointer());

    switch (role)
    {
        case Qt::EditRole:
        {
            switch (column)
            {
                case Column::Name:
                    return action->text();

                case Column::Type:
                    return action->getTypeString();

                case Column::IsPublic:
                    return action->isPublic();

                case Column::NumberOfConnections:
                    return QVariant::fromValue(_publicActions[row]->getConnectedActions().count());

                default:
                    break;
            }

            break;
        }

        case Qt::DisplayRole:
        {
            switch (column)
            {
                case Column::Name:
                    return data(index, Qt::EditRole).toString();

                case Column::Type:
                    return data(index, Qt::EditRole).toString();

                case Column::IsPublic:
                    return data(index, Qt::EditRole).toBool() ? "true" : "false";

                case Column::NumberOfConnections:
                    return QString::number(data(index, Qt::EditRole).toInt());

                default:
                    break;
            }

            break;
        }

        case Qt::ToolTipRole:
        {
            switch (column)
            {
                case Column::Name:
                    return data(index, Qt::EditRole).toString();

                case Column::Type:
                    return data(index, Qt::EditRole).toString();

                case Column::IsPublic:
                    return "Parameter is" + QString(data(index, Qt::EditRole).toBool() ? "" : "not") + " public";

                case Column::NumberOfConnections:
                {
                    QStringList connectedActionNames;

                    for (auto connectedAction : action->getConnectedActions())
                        connectedActionNames << connectedAction->getSettingsPath();

                    return QVariant::fromValue(connectedActionNames.join("\n"));
                }

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

bool ActionsModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
{
    const auto row      = index.row();
    const auto column   = index.column();
    const auto action   = static_cast<WidgetAction*>(index.internalPointer());

    switch (role) {
        case Qt::EditRole:
        {
            switch (column)
            {
                case Column::Name:
                    action->setText(value.toString());
                    break;

                case Column::Type:
                case Column::IsPublic:
                case Column::NumberOfConnections:
                    break;

                default:
                    break;
            }

            break;
        }

        default:
            break;
    }

    emit dataChanged(index, index);

    return true;
}

QVariant ActionsModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
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

                    case Column::IsPublic:
                        return "Public";

                    case Column::NumberOfConnections:
                        return "#Connections";

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
                        return "Public parameter name";

                    case Column::Type:
                        return "The type of parameter";

                    case Column::IsPublic:
                        return "Whether the parameter is public or not";

                    case Column::NumberOfConnections:
                        return "Number of connections to public parameter";

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

Qt::ItemFlags ActionsModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    auto itemFlags = Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | QAbstractItemModel::flags(index);

    if (index.column() == Column::Name)
        itemFlags |= Qt::ItemIsEditable;

    return itemFlags;
}
