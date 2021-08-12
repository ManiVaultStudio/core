#include "SharedActions.h"

#include <QDebug>
#include <QMenu>

namespace hdps {

namespace gui {

void SharedActions::addAction(gui::WidgetAction* widgetAction)
{
    Q_ASSERT(widgetAction != nullptr);

    _actions << widgetAction;
}

void SharedActions::removeAction(gui::WidgetAction* widgetAction)
{
    Q_ASSERT(widgetAction != nullptr);

    _actions.removeOne(widgetAction);
}

WidgetActions SharedActions::getActionsByContext(const QString& context) const
{
    WidgetActions actions;

    for (auto action : _actions)
        if (context == action->getContext())
            actions << action;

    return actions;
}

WidgetAction* SharedActions::getActionByName(const QString& name)
{
    for (auto action : _actions)
        if (action->text() == name)
            return action;

    return nullptr;
}

void SharedActions::populateContextMenu(const QString& context, QMenu* contextMenu)
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

QMenu* SharedActions::getContextMenu()
{
    auto menu = new QMenu();

    for (auto action : _actions) {
        auto contextMenu = action->getContextMenu();

        if (contextMenu == nullptr)
            continue;

        menu->addMenu(contextMenu);
    }

    return menu;

}

}
}