#include "ActionsModel.h"

#include "actions/WidgetAction.h"
#include "util/Exception.h"

using namespace hdps::gui;
using namespace hdps::util;

#ifdef _DEBUG
    #define ACTIONS_MODEL_VERBOSE
#endif

namespace hdps
{

void ActionsModel::addAction(WidgetAction* action)
{
#ifdef ACTIONS_MODEL_VERBOSE
    qDebug() << __FUNCTION__ << action->text();
#endif

    //try
    //{
    //    // Insert the layer action at the beginning
    //    beginInsertRows(QModelIndex(), 0, 0);
    //    {
    //        _publicActions.insert(0, action);

    //        const auto updateNumberOfConnectionsColumn = [this, action]() -> void {
    //            const auto changedCell = index(_publicActions.indexOf(action), Column::NumberOfConnections);
    //            emit dataChanged(changedCell, changedCell);
    //        };

    //        connect(action, &WidgetAction::actionConnected, this, updateNumberOfConnectionsColumn);
    //        connect(action, &WidgetAction::actionDisconnected, this, updateNumberOfConnectionsColumn);
    //    }
    //    endInsertRows();
    //}
    //catch (std::exception& e)
    //{
    //    exceptionMessageBox("Unable to add action to the public actions model", e);
    //}
    //catch (...) {
    //    exceptionMessageBox("Unable to add action to the public actions model");
    //}
}

void ActionsModel::removeAction(WidgetAction* action)
{
#ifdef ACTIONS_MODEL_VERBOSE
    qDebug() << __FUNCTION__ << action->text();
#endif

    //try
    //{
    //    if (!_publicActions.contains(action))
    //        throw std::runtime_error("Action does not exist");

    //    const auto row = _publicActions.indexOf(action);

    //    beginRemoveRows(QModelIndex(), row, row);
    //    {
    //        _publicActions.removeAt(row);

    //        delete action;
    //    }
    //    endRemoveRows();
    //}
    //catch (std::exception& e)
    //{
    //    exceptionMessageBox("Unable to remove action from the public actions model", e);
    //}
    //catch (...) {
    //    exceptionMessageBox("Unable to remove action from the public actions model");
    //}
}

}
