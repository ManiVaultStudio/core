#include "ActionsManager.h"

#include <models/ActionsFilterModel.h>
#include <actions/WidgetAction.h>
#include <util/Exception.h>

using namespace hdps::gui;
using namespace hdps::util;

namespace hdps
{

ActionsManager::ActionsManager() :
    AbstractActionsManager(),
    _model()
{
    setText("Actions manager");
    setObjectName("Actions");
}

ActionsManager::~ActionsManager()
{
}

void ActionsManager::addAction(WidgetAction* action)
{
    try
    {
        if (action == nullptr)
            throw std::runtime_error("Action is not valid");

        _model.addPublicAction(action);
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

        _model.removePublicAction(action);
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

const QAbstractItemModel& ActionsManager::getModel() const
{
    return _model;
}

bool ActionsManager::isActionPublic(const WidgetAction* action) const
{
    return _model._publicActions.contains(const_cast<WidgetAction*>(action));
}

bool ActionsManager::isActionPublished(const WidgetAction* action) const
{
    for (const auto publicAction : _model._publicActions)
        if (publicAction->getConnectedActions().first() == action)
            return true;

    return false;
}

bool ActionsManager::isActionConnected(const WidgetAction* action) const
{
    for (const auto publicAction : _model._publicActions)
        if (publicAction->getConnectedActions().contains(const_cast<WidgetAction*>(action)))
            return true;

    return false;
}

}
