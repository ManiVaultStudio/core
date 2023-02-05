#include "ActionsManager.h"

#include <Application.h>
#include <models/ActionsFilterModel.h>
#include <actions/WidgetAction.h>
#include <util/Exception.h>

using namespace hdps::gui;
using namespace hdps::util;

#ifdef _DEBUG
    //#define ACTIONS_MANAGER_VERBOSE
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

void ActionsManager::initialize()
{
#ifdef ACTIONS_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    AbstractManager::initialize();
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

void ActionsManager::fromVariantMap(const QVariantMap& variantMap)
{
    //Serializable::fromVariantMap(variantMap);

    //variantMapMustContain(variantMap, "PublicActions");

    //const auto publicActions = variantMap["PublicActions"].toList();
}

QVariantMap ActionsManager::toVariantMap() const
{
    auto variantMap = Serializable::toVariantMap();

    //QVariantList publicActions;

    //for (const auto& action : _actions) {
    //    if (!action->isPublic())
    //        continue;

    //    publicActions << action->toVariantMap();
    //}

    //variantMap.insert({
    //    { "PublicActions", publicActions }
    //});

    return variantMap;
}

}
