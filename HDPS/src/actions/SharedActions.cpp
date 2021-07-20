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

QMenu* SharedActions::getContextMenu()
{
    auto menu = new QMenu();

    for (auto action : _actions)
        menu->addMenu(action->getContextMenu());

    return menu;

}

}
}