#include "LoadSystemViewMenu.h"
#include "PluginManager.h"
#include "ViewPluginDockWidget.h"

#include <Application.h>
#include <AbstractLayoutManager.h>

#include <actions/PluginTriggerAction.h>

#include <util/Miscellaneous.h>

using namespace hdps;
using namespace hdps::plugin;
using namespace hdps::gui;
using namespace hdps::util;

LoadSystemViewMenu::LoadSystemViewMenu(QWidget *parent /*= nullptr*/, ads::CDockAreaWidget* dockAreaWidget /*= nullptr*/) :
    QMenu(parent),
    _dockAreaWidget(dockAreaWidget)
{
    setTitle("System views");
    setToolTip("Manage system view plugins");
}

void LoadSystemViewMenu::showEvent(QShowEvent* showEvent)
{
    clear();

    if (_dockAreaWidget) {
        const auto addLoadViewsDocked = [&](gui::DockAreaFlag dockArea) -> QMenu* {
            auto loadViewsDockedMenu = new QMenu(gui::dockAreaMap.key(dockArea));

            loadViewsDockedMenu->setIcon(getDockAreaIcon(dockArea));

            for (auto action : getLoadSystemViewsActions(dockArea))
                loadViewsDockedMenu->addAction(action);

            return loadViewsDockedMenu;
        };

        addMenu(addLoadViewsDocked(gui::DockAreaFlag::Left));
        addMenu(addLoadViewsDocked(gui::DockAreaFlag::Right));
        addMenu(addLoadViewsDocked(gui::DockAreaFlag::Top));
        addMenu(addLoadViewsDocked(gui::DockAreaFlag::Bottom));
        addMenu(addLoadViewsDocked(gui::DockAreaFlag::Center));
    }
    else {
        const auto actions = getLoadSystemViewsActions(gui::DockAreaFlag::Right);

        for (auto action : actions)
            addAction(action);
    }
}

QVector<QAction*> LoadSystemViewMenu::getLoadSystemViewsActions(hdps::gui::DockAreaFlag dockArea)
{
    QVector<QAction*> actions;

    auto pluginTriggerActions = Application::core()->getPluginManager().getPluginTriggerActions(Type::VIEW);

    for (auto pluginTriggerAction : pluginTriggerActions) {
        auto viewPluginFactory = dynamic_cast<const ViewPluginFactory*>(pluginTriggerAction->getPluginFactory());

        if (!viewPluginFactory->producesSystemViewPlugins())
            continue;

        auto action = new QAction(pluginTriggerAction->icon(), viewPluginFactory->getKind(), this);

        action->setEnabled(pluginTriggerAction->isEnabled());

        ViewPlugin* dockToViewPlugin = nullptr;

        if (_dockAreaWidget && _dockAreaWidget->dockWidgetsCount() >= 1) {
            auto dockWidgets = _dockAreaWidget->dockWidgets();

            auto firstViewPluginDockWidget = dynamic_cast<ViewPluginDockWidget*>(_dockAreaWidget->dockWidgets().first());

            if (firstViewPluginDockWidget)
                dockToViewPlugin = firstViewPluginDockWidget->getViewPlugin();
        }

        connect(action, &QAction::triggered, this, [pluginTriggerAction, dockToViewPlugin, dockArea]() -> void {
            auto viewPlugin = Application::core()->requestPlugin(pluginTriggerAction->getPluginFactory()->getKind());
            Application::core()->getLayoutManager().addViewPlugin(dynamic_cast<ViewPlugin*>(viewPlugin), dockToViewPlugin, dockArea);
        });

        actions << action;
    }

    sortActions(actions);

    return actions;
}
