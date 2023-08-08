// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ViewMenu.h"
#include "LoadSystemViewMenu.h"
#include "LoadedViewsMenu.h"
#include "PluginManager.h"
#include "ViewPluginDockWidget.h"

#include <Application.h>
#include <CoreInterface.h>
#include <AbstractWorkspaceManager.h>

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
    
    // macOS does not like populating the menu on show, so we rather do it explicitly here
    populate();

    const auto updateReadOnly = [this]() -> void {
        setEnabled(projects().hasProject() && !workspaces().getLockingAction().isLocked());
    };

    updateReadOnly();

    connect(&projects(), &AbstractProjectManager::projectCreated, this, updateReadOnly);
    connect(&projects(), &AbstractProjectManager::projectDestroyed, this, updateReadOnly);
    connect(&workspaces().getLockingAction(), &LockingAction::lockedChanged, this, updateReadOnly);
}

void ViewMenu::populate()
{
    for (auto action : actions())
        if (action != &projects().getCurrentProject()->getStudioModeAction())
            delete action;

    clear();
    
    if (_options.testFlag(LoadSystemViewPlugins))
        addMenu(new LoadSystemViewMenu());

    Application::processEvents();

    auto separator = addSeparator();

    Application::processEvents();

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
            const auto actions = getLoadViewsActions(gui::DockAreaFlag::Right);

            for (auto action : actions)
                insertAction(_separator, action);
        }
    }

    _separator = addSeparator();

    if (_options.testFlag(LoadedViewsSubMenu))
        addMenu(new LoadedViewsMenu());

    if (projects().hasProject()) {
        addSeparator();
        addAction(&projects().getCurrentProject()->getStudioModeAction());
    }
}

bool ViewMenu::mayProducePlugins() const
{
    for (auto pluginTriggerAction : plugins().getPluginTriggerActions(Type::VIEW)) {
        auto viewPluginFactory = dynamic_cast<const ViewPluginFactory*>(pluginTriggerAction->getPluginFactory());

        if (viewPluginFactory->producesSystemViewPlugins())
            continue;

        if (pluginTriggerAction->isEnabled())
            return true;
    }

    return false;
}

QVector<QPointer<PluginTriggerAction>> ViewMenu::getLoadViewsActions(gui::DockAreaFlag dockArea)
{
    PluginTriggerActions pluginTriggerActions;

    for (auto pluginTriggerAction : plugins().getPluginTriggerActions(plugin::Type::VIEW)) {
        auto viewPluginFactory = dynamic_cast<const ViewPluginFactory*>(pluginTriggerAction->getPluginFactory());

        if (viewPluginFactory->producesSystemViewPlugins())
            continue;
            
        pluginTriggerActions << new PluginTriggerAction(*pluginTriggerAction, QString("Create %1").arg(viewPluginFactory->getKind()));

        ViewPlugin* dockToViewPlugin = nullptr;

        if (_dockAreaWidget && _dockAreaWidget->dockWidgetsCount() >= 1) {
            auto dockWidgets = _dockAreaWidget->dockWidgets();

            auto firstViewPluginDockWidget = dynamic_cast<ViewPluginDockWidget*>(_dockAreaWidget->dockWidgets().first());

            if (firstViewPluginDockWidget)
                dockToViewPlugin = firstViewPluginDockWidget->getViewPlugin();
        }

        pluginTriggerActions.last()->setRequestPluginCallback([dockToViewPlugin, dockArea](PluginTriggerAction& pluginTriggerAction) -> void {
            plugins().requestViewPlugin(pluginTriggerAction.getPluginFactory()->getKind(), dockToViewPlugin, dockArea);
        });
    }

    sortActions(pluginTriggerActions);

    return pluginTriggerActions;
}
