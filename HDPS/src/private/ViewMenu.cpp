#include "ViewMenu.h"
#include "LoadSystemViewMenu.h"
#include "LoadedViewsMenu.h"
#include "PluginManager.h"
#include "ViewPluginDockWidget.h"

#include <Application.h>
#include <AbstractLayoutManager.h>

#include <util/Miscellaneous.h>

using namespace hdps;
using namespace hdps::plugin;
using namespace hdps::gui;
using namespace hdps::util;

ViewMenu::ViewMenu(QWidget *parent /*= nullptr*/, const Options& options /*= Option::Default*/, ads::CDockAreaWidget* dockAreaWidget /*= nullptr*/) :
    QMenu(parent),
    _dockAreaWidget(dockAreaWidget),
    _options(options),
    _separator(nullptr)
{
    setTitle("View");
    setToolTip("Manage view plugins");
}

void ViewMenu::showEvent(QShowEvent* showEvent)
{
    clear();

    if (_options.testFlag(LoadSystemViewPlugins)) 
        addMenu(new LoadSystemViewMenu());

    auto separator = addSeparator();

    if (_dockAreaWidget) {
        const auto addLoadViewsDocked = [&](gui::DockAreaFlag dockArea) -> QMenu* {
            auto loadViewsDockedMenu = new QMenu(gui::dockAreaMap.key(dockArea));

            loadViewsDockedMenu->setIcon(getDockAreaIcon(dockArea));

            for (auto action : getLoadViewsActions(dockArea))
                loadViewsDockedMenu->addAction(action);

            return loadViewsDockedMenu;
        };

        insertMenu(_separator, addLoadViewsDocked(gui::DockAreaFlag::Left));
        insertMenu(_separator, addLoadViewsDocked(gui::DockAreaFlag::Right));
        insertMenu(_separator, addLoadViewsDocked(gui::DockAreaFlag::Top));
        insertMenu(_separator, addLoadViewsDocked(gui::DockAreaFlag::Bottom));
        insertMenu(_separator, addLoadViewsDocked(gui::DockAreaFlag::Center));
    }
    else {
        if (_options.testFlag(LoadViewPlugins)) {
            const auto actions = getLoadViewsActions(gui::DockAreaFlag::None);

            for (auto action : actions)
                insertAction(_separator, action);
        }
    }

    _separator = addSeparator();

    if (_options.testFlag(LoadedViewsSubMenu))
        addMenu(new LoadedViewsMenu());
}

bool ViewMenu::mayProducePlugins() const
{
    for (auto pluginTriggerAction : Application::core()->getPluginManager().getPluginTriggerActions(Type::VIEW)) {
        auto viewPluginFactory = dynamic_cast<const ViewPluginFactory*>(pluginTriggerAction->getPluginFactory());

        if (viewPluginFactory->producesSystemViewPlugins())
            continue;

        if (pluginTriggerAction->isEnabled())
            return true;
    }

    return false;
}

QVector<QAction*> ViewMenu::getLoadViewsActions(gui::DockAreaFlag dockArea)
{
    QVector<QAction*> actions;

    auto pluginTriggerActions = Application::core()->getPluginManager().getPluginTriggerActions(plugin::Type::VIEW);

    for (auto pluginTriggerAction : pluginTriggerActions) {
        auto viewPluginFactory = dynamic_cast<const ViewPluginFactory*>(pluginTriggerAction->getPluginFactory());

        if (viewPluginFactory->producesSystemViewPlugins())
            continue;

        auto action = new QAction(pluginTriggerAction->icon(), viewPluginFactory->getKind(), this);

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
