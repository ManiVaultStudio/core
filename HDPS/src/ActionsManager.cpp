#include "ActionsManager.h"

#include "actions/WidgetAction.h"
#include "util/Exception.h"

using namespace hdps::gui;
using namespace hdps::util;

namespace hdps
{

ActionsManager::ActionsManager(QObject* parent /*= nullptr*/) :
    QObject(parent),
    _publicActionsModel()
{
}

void ActionsManager::addPublicAction(WidgetAction* publicAction)
{
    try
    {
        if (publicAction == nullptr)
            throw std::runtime_error("Action is not valid");

        _publicActionsModel.addPublicAction(publicAction);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to add public action ", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to add public action ");
    }
}

void ActionsManager::removePublicAction(WidgetAction* publicAction)
{
    try
    {
        if (publicAction == nullptr)
            throw std::runtime_error("Action is not valid");

        _publicActionsModel.removePublicAction(publicAction);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to remove public action ", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to remove public action ");
    }
}

const ActionsManager::PublicActionsModel& ActionsManager::getPublicActionsModel() const
{
    return _publicActionsModel;
}

bool ActionsManager::isActionPublic(const WidgetAction* action) const
{
    return _publicActionsModel._publicActions.contains(const_cast<WidgetAction*>(action));
}

bool ActionsManager::isActionPublished(const WidgetAction* action) const
{
    for (const auto publicAction : _publicActionsModel._publicActions)
        if (publicAction->getConnectedActions().first() == action)
            return true;

    return false;
}

bool ActionsManager::isActionConnected(const WidgetAction* action) const
{
    for (const auto publicAction : _publicActionsModel._publicActions)
        if (publicAction->getConnectedActions().contains(const_cast<WidgetAction*>(action)))
            return true;

    return false;
}

int ActionsManager::PublicActionsModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
    return _publicActions.count();
}

int ActionsManager::PublicActionsModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
    return 2;
}

QModelIndex ActionsManager::PublicActionsModel::index(int row, int column, const QModelIndex& parent /*= QModelIndex()*/) const
{
    return createIndex(row, column, static_cast<void*>(_publicActions.at(row)));
}

void ActionsManager::PublicActionsModel::addPublicAction(WidgetAction* publicAction)
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

void ActionsManager::PublicActionsModel::removePublicAction(WidgetAction* publicAction)
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

QVariant ActionsManager::PublicActionsModel::data(const QModelIndex& index, int role) const
{
    const auto row          = index.row();
    const auto column       = index.column();
    const auto publicAction = static_cast<WidgetAction*>(index.internalPointer());

    switch (role)
    {
        //case Qt::DecorationRole:
        //    return getDecorationRole(lineType);

        case Qt::EditRole:
        {
            switch (column)
            {
                case Column::Name:
                    return _publicActions[row]->text();

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

                case Column::NumberOfConnections:
                {
                    QStringList connectedActionNames;

                    for (auto connectedAction : publicAction->getConnectedActions())
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

QVariant ActionsManager::PublicActionsModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
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

}
