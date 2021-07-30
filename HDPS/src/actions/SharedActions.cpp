#include "SharedActions.h"

#include <QDebug>
#include <QMenu>

namespace hdps {

namespace gui {

void SharedActions::exposeAction(gui::WidgetAction* widgetAction)
{
    Q_ASSERT(widgetAction != nullptr);

    _actions << widgetAction;
}

void SharedActions::concealAction(gui::WidgetAction* widgetAction)
{
    Q_ASSERT(widgetAction != nullptr);

    _actions.removeOne(widgetAction);
}

const hdps::gui::SharedActions::WidgetActionList& SharedActions::getExposedActions() const
{
    return _actions;
}

hdps::gui::WidgetAction* SharedActions::getActionByName(const QString& name)
{
    for (auto action : _actions)
        if (action->text() == name)
            return action;

    return nullptr;
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