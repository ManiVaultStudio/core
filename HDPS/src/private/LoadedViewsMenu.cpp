#include "LoadedViewsMenu.h"

#include <ViewPlugin.h>

#include <actions/PluginTriggerAction.h>

#include <util/Miscellaneous.h>

using namespace hdps;
using namespace hdps::plugin;
using namespace hdps::util;

LoadedViewsMenu::LoadedViewsMenu(QWidget *parent /*= nullptr*/) :
    QMenu(parent)
{
    setTitle("Loaded");
    setToolTip("Manage loaded view plugins");
}

void LoadedViewsMenu::showEvent(QShowEvent* showEvent)
{
    clear();

    QVector<QAction*> actions;

    const auto plugins = Application::core()->getPluginsByType({ plugin::Type::VIEW });

    for (auto plugin : plugins)
        actions << &dynamic_cast<ViewPlugin*>(plugin)->getVisibleAction();

    sortActions(actions);

    for (auto action : actions)
        addAction(action);
}
