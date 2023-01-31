#include "LoadedViewsMenu.h"

#include <Application.h>
#include <CoreInterface.h>
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
    setEnabled(!plugins().getPluginsByType(plugin::Type::VIEW).empty());
    setIcon(Application::getIconFont("FontAwesome").getIcon("low-vision"));

    connect(this, &QMenu::aboutToShow, this, [this]() -> void {
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
    });
}

QVector<QPointer<ToggleAction>> LoadedViewsMenu::getLoadedViewsActions(bool systemView)
{
    QVector<QPointer<ToggleAction>> actions;

    for (auto plugin : plugins().getPluginsByType(plugin::Type::VIEW)) {
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
