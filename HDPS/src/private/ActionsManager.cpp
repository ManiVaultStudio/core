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
    _publicActions()
{
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

const hdps::gui::WidgetActions& ActionsManager::getActions() const
{
    return _actions;
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

void ActionsManager::addPublicAction(gui::WidgetAction* action)
{
    Q_ASSERT(action != nullptr);

#ifdef ACTIONS_MANAGER_VERBOSE
    qDebug() << __FUNCTION__ << action->text();
#endif

    _publicActions << action;

    emit publicActionAdded(action);
}

void ActionsManager::removePublicAction(gui::WidgetAction* action)
{
    Q_ASSERT(action != nullptr);

#ifdef ACTIONS_MANAGER_VERBOSE
    qDebug() << __FUNCTION__ << action->text();
#endif

    const auto actionId = action->getId();

    emit publicActionAboutToBeRemoved(action);
    {
        if (_publicActions.contains(action))
            _publicActions.removeOne(action);
    }
    emit publicActionRemoved(actionId);
}

void ActionsManager::fromVariantMap(const QVariantMap& variantMap)
{
    Serializable::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "PublicActions");

    const auto publicActions = variantMap["PublicActions"].toList();

    for (const auto& publicAction : publicActions) {
        const auto publicActionMap  = publicAction.toMap();
        const auto metaType         = publicAction.toMap()["Type"].toString();
        const auto metaTypeId       = QMetaType::type(metaType.toLatin1());
        const auto metaObject       = QMetaType::metaObjectForType(metaTypeId);

        if (metaObject) {
            auto action = qobject_cast<WidgetAction*>(metaObject->newInstance());

            action->setText(publicActionMap["Title"].toString());
            action->fromVariantMap(publicActionMap);

            makeActionPublic(action);
        }
    }
}

QVariantMap ActionsManager::toVariantMap() const
{
    auto variantMap = Serializable::toVariantMap();

    QVariantList publicActions;

    for (const auto action : _actions) {
        if (!action->isPublic())
            continue;

        auto actionVariantMap = action->toVariantMap();

        actionVariantMap["Title"] = action->text();

        publicActions << actionVariantMap;
    }

    variantMap.insert({
        { "PublicActions", publicActions }
    });

    return variantMap;
}

WidgetAction* ActionsManager::getAction(const QString& id)
{
    for (const auto action : _actions)
        if (id == action->getId())
            return action;
    
    return nullptr;
}

}
