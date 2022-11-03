#include "ActionsManager.h"

#include "actions/WidgetAction.h"
#include "util/Exception.h"

using namespace hdps::gui;
using namespace hdps::util;

namespace hdps
{

ActionsManager::ActionsManager(QObject* parent /*= nullptr*/) :
    QObject(parent),
    _actionsModel()
{
}

void ActionsManager::addAction(WidgetAction* action)
{
    try
    {
        if (action == nullptr)
            throw std::runtime_error("Action is not valid");

        _actionsModel.addPublicAction(action);
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

void ActionsManager::removeAction(WidgetAction* action)
{
    try
    {
        if (action == nullptr)
            throw std::runtime_error("Action is not valid");

        _actionsModel.removePublicAction(action);
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

const ActionsModel& ActionsManager::getActionsModel() const
{
    return _actionsModel;
}

bool ActionsManager::isActionPublic(const WidgetAction* action) const
{
    return _actionsModel._publicActions.contains(const_cast<WidgetAction*>(action));
}

bool ActionsManager::isActionPublished(const WidgetAction* action) const
{
    for (const auto publicAction : _actionsModel._publicActions)
        if (publicAction->getConnectedActions().first() == action)
            return true;

    return false;
}

bool ActionsManager::isActionConnected(const WidgetAction* action) const
{
    for (const auto publicAction : _actionsModel._publicActions)
        if (publicAction->getConnectedActions().contains(const_cast<WidgetAction*>(action)))
            return true;

    return false;
}

}
