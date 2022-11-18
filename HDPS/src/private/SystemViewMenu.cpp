#include "ViewMenu.h"
#include "PluginManager.h"
#include "ViewPluginDockWidget.h"

#include <Application.h>
#include <AbstractLayoutManager.h>

#include <actions/PluginTriggerAction.h>
#include <actions/ViewPluginTriggerAction.h>

#include <algorithm>

using namespace std;

using namespace hdps;
using namespace hdps::plugin;
using namespace hdps::gui;

auto sortActions = [](QVector<QAction*>& actions) -> void {
    sort(actions.begin(), actions.end(), [](auto actionA, auto actionB) {
        return actionA->text() < actionB->text();
    });
};

ViewMenu::ViewMenu(QWidget *parent /*= nullptr*/, const Options& options /*= Option::Default*/, ads::CDockAreaWidget* dockAreaWidget /*= nullptr*/) :
    QMenu(parent),
    _dockAreaWidget(dockAreaWidget),
    _options(options),
    _systemViewsMenu(this),
    _loadedViewsMenu(this),
    _separator(nullptr)
{
    setTitle("View");
    setToolTip("Manage view plugins");

    _systemViewsMenu.setTitle("Standard");
    _loadedViewsMenu.setTitle("Loaded");

    _systemViewsMenu.setEnabled(false);
    _loadedViewsMenu.setEnabled(false);

    _separator = addSeparator();

    _separator->setVisible(false);

    if (options.testFlag(LoadSystemViewPlugins))
        addMenu(&_systemViewsMenu);

    if (options.testFlag(LoadedViewsSubMenu))
        addMenu(&_loadedViewsMenu);
}

void ViewMenu::showEvent(QShowEvent* showEvent)
{
    const auto pluginTriggerActions = Application::core()->getPluginManager().getPluginTriggerActions(plugin::Type::VIEW);

    if (_dockAreaWidget) {
        const auto addLoadViewsDocked = [&](gui::DockAreaFlag dockArea) -> QMenu* {
            QMenu* loadViewsDockedMenu = new QMenu(gui::dockAreaMap.key(static_cast<std::uint32_t>(dockArea)));

            for (auto action : getLoadViewsActions(pluginTriggerActions, dockArea))
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
            const auto actions = getLoadViewsActions(pluginTriggerActions, gui::DockAreaFlag::None);

            for (auto action : actions)
                insertAction(_separator, action);
        }
            
        if (_options.testFlag(LoadSystemViewPlugins)) {
            const auto actions = getLoadSystemViewsActions(pluginTriggerActions, gui::DockAreaFlag::None);

            for (auto action : actions)
                _systemViewsMenu.addAction(action);

            _systemViewsMenu.setEnabled(!actions.isEmpty());
        }
    }

    //_separator->setVisible(!pluginTriggerActions.isEmpty() && (_options.testFlag(LoadStandardViewSubMenu) || _options.testFlag(LoadedViewsSubMenu)));
}

QVector<QAction*> ViewMenu::getLoadViewsActions(const hdps::gui::PluginTriggerActions& pluginTriggerActions, gui::DockAreaFlag dockArea)
{
    QVector<QAction*> actions;

    for (auto pluginTriggerAction : pluginTriggerActions) {
        auto viewPluginTriggerAction    = dynamic_cast<ViewPluginTriggerAction*>(pluginTriggerAction);
        auto viewPluginFactory          = dynamic_cast<const ViewPluginFactory*>(viewPluginTriggerAction->getPluginFactory());

        if (_dockAreaWidget && _dockAreaWidget->dockWidgetsCount() >= 1) {
            auto dockWidgets = _dockAreaWidget->dockWidgets();

            auto firstViewPluginDockWidget = dynamic_cast<ViewPluginDockWidget*>(_dockAreaWidget->dockWidgets().first());

            viewPluginTriggerAction->setDockToViewPlugin(firstViewPluginDockWidget->getViewPlugin());
            viewPluginTriggerAction->setDockArea(dockArea);
        }
    

        if (viewPluginFactory == nullptr)
            continue;

        if (!viewPluginFactory->producesSystemViewPlugins())
            actions << pluginTriggerAction;
    }

    sortActions(actions);

    return actions;
}

QVector<QAction*> ViewMenu::getLoadSystemViewsActions(const hdps::gui::PluginTriggerActions& pluginTriggerActions, gui::DockAreaFlag dockArea)
{
    QVector<QAction*> actions;

    for (const auto& pluginTriggerAction : pluginTriggerActions) {
        auto viewPluginFactory = dynamic_cast<const ViewPluginFactory*>(pluginTriggerAction->getPluginFactory());

        if (viewPluginFactory == nullptr)
            continue;

        if (viewPluginFactory->producesSystemViewPlugins())
            actions << pluginTriggerAction;
    }

    sortActions(actions);

    return actions;
}
