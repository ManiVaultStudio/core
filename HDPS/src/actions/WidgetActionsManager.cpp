#include "WidgetActionsManager.h"

#include <QDebug>
#include <QMenu>

namespace hdps {

namespace gui {

void WidgetActionsManager::addAction(gui::WidgetAction* widgetAction)
{
    Q_ASSERT(widgetAction != nullptr);

    _actions << widgetAction;
}

void WidgetActionsManager::removeAction(gui::WidgetAction* widgetAction)
{
    Q_ASSERT(widgetAction != nullptr);

    _actions.removeOne(widgetAction);
}

WidgetActions WidgetActionsManager::getActionsByContext(const QString& context) const
{
    WidgetActions actions;

    for (auto action : _actions)
        if (context == action->getContext())
            actions << action;

    return actions;
}

WidgetAction* WidgetActionsManager::getActionByName(const QString& name)
{
    for (auto action : _actions)
        if (action->text() == name)
            return action;

    return nullptr;
}

void WidgetActionsManager::populateContextMenu(const QString& context, QMenu* contextMenu)
{
    for (auto action : _actions) {
        if (context != action->getContext())
            continue;

        auto actionContextMenu = action->getContextMenu();

        if (actionContextMenu == nullptr)
            continue;

        contextMenu->addMenu(actionContextMenu);
    }
}

QMenu* WidgetActionsManager::getContextMenu(QWidget* parent, const QString& context)
{
    auto menu = new QMenu(parent);

    for (auto action : _actions) {
        if (context != action->getContext())
            continue;

        auto contextMenu = action->getContextMenu();

        if (contextMenu == nullptr)
            continue;

        menu->addMenu(contextMenu);
    }

    return menu;
}

void WidgetActionsManager::widgetStartedDragging(WidgetAction* widgetAction)
{
    /*
    for (auto action : _actions) {
        qDebug() << action;

        if (widgetAction == action)
            continue;

        if (typeid(*widgetAction).name() != typeid(*action).name())
            continue;

        action->setDropTarget(true);
    }
    */
}

void WidgetActionsManager::widgetStoppedDragging(WidgetAction* widgetAction)
{
    /*
    for (auto action : _actions) {
        if (widgetAction == action)
            continue;

        if (typeid(*widgetAction).name() != typeid(*action).name())
            continue;

        action->setDropTarget(false);
    }
    */
}

}
}
