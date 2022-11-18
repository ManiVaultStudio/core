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
    _loadedViewsMenu(this),
    _separator(nullptr)
{
    setTitle("View");
    setToolTip("Manage view plugins");
}

void ViewMenu::showEvent(QShowEvent* showEvent)
{
    clear();

    _loadedViewsMenu.setTitle("Loaded");
    _loadedViewsMenu.setEnabled(false);

    if (_dockAreaWidget) {
        const auto addLoadViewsDocked = [&](gui::DockAreaFlag dockArea) -> QMenu* {
            auto loadViewsDockedMenu = new QMenu(gui::dockAreaMap.key(dockArea));

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

    if (_options.testFlag(LoadedViewsSubMenu)) {
        _separator = addSeparator();
        
        _separator->setVisible(false);

        addMenu(&_loadedViewsMenu);
    }
}

QVector<QAction*> ViewMenu::getLoadViewsActions(gui::DockAreaFlag dockArea)
{
    QVector<QAction*> actions;

    auto loadViewPluginTriggerActions = Application::core()->getPluginManager().getPluginTriggerActions(plugin::Type::VIEW);

    for (auto loadViewPluginTriggerAction : loadViewPluginTriggerActions) {
        auto viewPluginFactory          = dynamic_cast<const ViewPluginFactory*>(loadViewPluginTriggerAction->getPluginFactory());
        auto action                     = new QAction(loadViewPluginTriggerAction->icon(), viewPluginFactory->getKind(), this);

        ViewPlugin* dockToViewPlugin = nullptr;

        if (_dockAreaWidget && _dockAreaWidget->dockWidgetsCount() >= 1) {
            auto dockWidgets = _dockAreaWidget->dockWidgets();

            auto firstViewPluginDockWidget = dynamic_cast<ViewPluginDockWidget*>(_dockAreaWidget->dockWidgets().first());

            if (firstViewPluginDockWidget)
                dockToViewPlugin = firstViewPluginDockWidget->getViewPlugin();
        }

        connect(action, &QAction::triggered, this, [loadViewPluginTriggerAction, dockToViewPlugin, dockArea]() -> void {
            auto viewPlugin = Application::core()->requestPlugin(loadViewPluginTriggerAction->getPluginFactory()->getKind());
            Application::core()->getLayoutManager().addViewPlugin(dynamic_cast<ViewPlugin*>(viewPlugin), dockToViewPlugin, dockArea);
        });

        if (viewPluginFactory == nullptr)
            continue;

        if (!viewPluginFactory->producesSystemViewPlugins())
            actions << action;
    }

    sortActions(actions);

    return actions;
}
