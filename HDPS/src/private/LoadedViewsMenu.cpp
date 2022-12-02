#include "LoadedViewsMenu.h"

#include <ViewPlugin.h>

#include <actions/PluginTriggerAction.h>

#include <util/Miscellaneous.h>

using namespace hdps;
using namespace hdps::plugin;
using namespace hdps::util;
using namespace hdps::gui;

LoadedViewsMenu::LoadedViewsMenu(QWidget *parent /*= nullptr*/) :
    QMenu(parent)
{
    setTitle("Toggle");
    setToolTip("Toggle loaded view plugin visibility");
    setEnabled(!Application::core()->getPluginsByType({ plugin::Type::VIEW }).isEmpty());
    setIcon(Application::getIconFont("FontAwesome").getIcon("low-vision"));
}

void LoadedViewsMenu::showEvent(QShowEvent* showEvent)
{
    clear();

    for (auto action : getLoadedViewsActions(false))
        addAction(action);

    if (!actions().isEmpty())
        addSeparator();

    const auto loadedViewActions = getLoadedViewsActions(true);

    if (!loadedViewActions.isEmpty()) {
        auto loadedSystemViewsMenu = new QMenu("System");

        for (auto loadedSystemViewAction : loadedViewActions)
            loadedSystemViewsMenu->addAction(loadedSystemViewAction);

        loadedSystemViewsMenu->setIcon(Application::getIconFont("FontAwesome").getIcon("cogs"));

        addMenu(loadedSystemViewsMenu);
    }
}

QVector<QPointer<ToggleAction>> LoadedViewsMenu::getLoadedViewsActions(bool systemView)
{
    QVector<QPointer<ToggleAction>> actions;

    const auto plugins = Application::core()->getPluginsByType({ plugin::Type::VIEW });

    for (auto plugin : plugins) {
        auto viewPlugin = dynamic_cast<ViewPlugin*>(plugin);

        if (systemView) {
            if (!viewPlugin->isSystemViewPlugin())
                continue;
        }
        else {
            if (viewPlugin->isSystemViewPlugin())
                continue;
        }

        actions << &viewPlugin->getVisibleAction();
    }

    sortActions(actions);

    return actions;
}
