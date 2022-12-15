#include "ActionsManager.h"

#include <Application.h>
#include <models/ActionsFilterModel.h>
#include <actions/WidgetAction.h>
#include <util/Exception.h>

using namespace hdps::gui;
using namespace hdps::util;

#ifdef _DEBUG
    #define ACTIONS_MANAGER_VERBOSE
#endif

namespace hdps
{

ActionsManager::ActionsManager() :
    AbstractActionsManager(),
    _actions(),
    _model()
{
    setObjectName("Actions");
}

ActionsManager::~ActionsManager()
{
    reset();
}

void ActionsManager::initalize()
{
#ifdef ACTIONS_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif
}

void ActionsManager::reset()
{
#ifdef ACTIONS_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    beginReset();
    {
    }
    endReset();
}

void ActionsManager::addAction(WidgetAction* action)
{
    Q_ASSERT(action != nullptr);

#ifdef ACTIONS_MANAGER_VERBOSE
    qDebug() << __FUNCTION__ << action->text();
#endif

    _actions << action;

    emit actionAdded(action);
}

void ActionsManager::removeAction(WidgetAction* action)
{
    Q_ASSERT(action != nullptr);

#ifdef ACTIONS_MANAGER_VERBOSE
    qDebug() << __FUNCTION__ << action->text();
#endif

    const auto actionId = action->getId();

    emit actionAboutToBeRemoved(action);
    {
        if (_actions.contains(action))
            _actions.removeOne(action);
    }
    emit actionRemoved(actionId);
}

const QAbstractItemModel& ActionsManager::getModel() const
{
    return _model;
}

}
