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
    setEnabled(mayProducePlugins());
    setIcon(Application::getIconFont("FontAwesome").getIcon("cogs"));
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

bool LoadSystemViewMenu::mayProducePlugins() const
{
    for (auto pluginTriggerAction : Application::core()->getPluginManager().getPluginTriggerActions(Type::VIEW)) {
        auto viewPluginFactory = dynamic_cast<const ViewPluginFactory*>(pluginTriggerAction->getPluginFactory());

        if (!viewPluginFactory->producesSystemViewPlugins())
            continue;

        if (pluginTriggerAction->isEnabled())
            return true;
    }

    return false;
}

QVector<QPointer<TriggerAction>> LoadSystemViewMenu::getLoadSystemViewsActions(hdps::gui::DockAreaFlag dockArea)
{
    QVector<QPointer<TriggerAction>> actions;

    auto pluginTriggerActions = Application::core()->getPluginManager().getPluginTriggerActions(Type::VIEW);

    for (auto pluginTriggerAction : pluginTriggerActions) {
        auto viewPluginFactory = dynamic_cast<const ViewPluginFactory*>(pluginTriggerAction->getPluginFactory());

        if (!viewPluginFactory->producesSystemViewPlugins())
            continue;

        auto action = new TriggerAction(this, viewPluginFactory->getKind());

        action->setIcon(pluginTriggerAction->icon());
        action->setEnabled(pluginTriggerAction->isEnabled());

        ViewPlugin* dockToViewPlugin = nullptr;

        if (_dockAreaWidget && _dockAreaWidget->dockWidgetsCount() >= 1) {
            auto dockWidgets = _dockAreaWidget->dockWidgets();

            auto firstViewPluginDockWidget = dynamic_cast<ViewPluginDockWidget*>(_dockAreaWidget->dockWidgets().first());

            if (firstViewPluginDockWidget)
                dockToViewPlugin = firstViewPluginDockWidget->getViewPlugin();
        }

        connect(action, &QAction::triggered, this, [pluginTriggerAction, dockToViewPlugin, dockArea]() -> void {
            Application::core()->requestViewPlugin(pluginTriggerAction->getPluginFactory()->getKind(), dockToViewPlugin, dockArea);
        });

        actions << action;
    }

    sortActions(actions);

    return actions;
}