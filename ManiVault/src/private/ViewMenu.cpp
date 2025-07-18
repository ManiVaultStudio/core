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

using namespace mv;
using namespace mv::plugin;
using namespace mv::gui;
using namespace mv::util;

ViewMenu::ViewMenu(QWidget *parent /*= nullptr*/, const Options& options /*= Option::Default*/, ads::CDockAreaWidget* dockAreaWidget /*= nullptr*/) :
    QMenu(parent),
    _dockAreaWidget(dockAreaWidget),
    _options(options),
    _separator(nullptr)
{
    setTitle("View");
    setToolTip("Manage view plugins");
    
    // the menu needs to be updated, e.g. for when new view actions are opened
    connect(this, &QMenu::aboutToShow, this, &ViewMenu::populate);

    // create menus once at start and re-populate them on aboutToShow
    _loadSystemViewMenu = QSharedPointer<LoadSystemViewMenu>(new LoadSystemViewMenu(this));
    _loadedViewsMenu = QSharedPointer<LoadedViewsMenu>(new LoadedViewsMenu(this));
    
    _loadViewsDockedMenus.insert(gui::DockAreaFlag::Left,   QSharedPointer<QMenu>(new QMenu(gui::dockAreaMap.key(gui::DockAreaFlag::Left), this)));
    _loadViewsDockedMenus.insert(gui::DockAreaFlag::Right,  QSharedPointer<QMenu>(new QMenu(gui::dockAreaMap.key(gui::DockAreaFlag::Right), this)));
    _loadViewsDockedMenus.insert(gui::DockAreaFlag::Top,    QSharedPointer<QMenu>(new QMenu(gui::dockAreaMap.key(gui::DockAreaFlag::Top), this)));
    _loadViewsDockedMenus.insert(gui::DockAreaFlag::Bottom, QSharedPointer<QMenu>(new QMenu(gui::dockAreaMap.key(gui::DockAreaFlag::Bottom), this)));
    _loadViewsDockedMenus.insert(gui::DockAreaFlag::Center, QSharedPointer<QMenu>(new QMenu(gui::dockAreaMap.key(gui::DockAreaFlag::Center), this)));

    populate();

    const auto updateReadOnly = [this]() -> void {
        setEnabled(projects().hasProject());
    };

    updateReadOnly();

    connect(&projects(), &AbstractProjectManager::projectCreated, this, updateReadOnly);
    connect(&projects(), &AbstractProjectManager::projectDestroyed, this, updateReadOnly);
    connect(&workspaces().getLockingAction(), &LockingAction::lockedChanged, this, updateReadOnly);
}

void ViewMenu::populate()
{
    clear();
    
    if (_options.testFlag(LoadSystemViewPlugins))
    {
        _loadSystemViewMenu->populate();
        addMenu(_loadSystemViewMenu.get());
    }

    if (_dockAreaWidget) {

        for (auto& dockArea : { gui::DockAreaFlag::Left, gui::DockAreaFlag::Right, gui::DockAreaFlag::Top, gui::DockAreaFlag::Bottom, gui::DockAreaFlag::Center })
        {
            auto& loadViewsDockedMenu = _loadViewsDockedMenus[dockArea];

            loadViewsDockedMenu->clear();
            loadViewsDockedMenu->setIcon(getDockAreaIcon(dockArea));

            for (auto& action : getLoadViewsActions(dockArea))
                loadViewsDockedMenu->addAction(action);

            insertMenu(_separator, loadViewsDockedMenu.get());
        }
    }
    else {
        if (_options.testFlag(LoadViewPlugins)) {
            const auto actions = getLoadViewsActions(gui::DockAreaFlag::Right);

            for (auto& action : actions)
                insertAction(_separator, action);
        }
    }

    _separator = addSeparator();

    if (_options.testFlag(LoadedViewsSubMenu))
    {
        _loadedViewsMenu->populate();
        addMenu(_loadedViewsMenu.get());
    }

    if (projects().hasProject()) {
        addSeparator();
        addAction(&projects().getCurrentProject()->getStudioModeAction());
    }
}

bool ViewMenu::mayProducePlugins() const
{
    for (auto& pluginTriggerAction : plugins().getPluginTriggerActions(Type::VIEW)) {
        auto viewPluginFactory = dynamic_cast<const ViewPluginFactory*>(pluginTriggerAction->getPluginFactory());

        if (viewPluginFactory->producesSystemViewPlugins())
            continue;

        if (pluginTriggerAction->isEnabled())
            return true;
    }

    return false;
}

QVector<QPointer<PluginTriggerAction>> ViewMenu::getLoadViewsActions(gui::DockAreaFlag dockArea) const
{
    PluginTriggerActions pluginTriggerActions;

    for (auto& pluginTriggerAction : plugins().getPluginTriggerActions(plugin::Type::VIEW)) {
        auto viewPluginFactory = dynamic_cast<const ViewPluginFactory*>(pluginTriggerAction->getPluginFactory());

        if (viewPluginFactory->producesSystemViewPlugins() || !viewPluginFactory->getAllowPluginCreationFromStandardGui())
            continue;
            
        pluginTriggerActions << new PluginTriggerAction(*pluginTriggerAction, QString("Create %1").arg(viewPluginFactory->getKind()));

        ViewPlugin* dockToViewPlugin = nullptr;

        if (_dockAreaWidget && _dockAreaWidget->dockWidgetsCount() >= 1) {
            auto dockWidgets = _dockAreaWidget->dockWidgets();

            if (auto firstViewPluginDockWidget = dynamic_cast<ViewPluginDockWidget*>(_dockAreaWidget->dockWidgets().first()))
                dockToViewPlugin = firstViewPluginDockWidget->getViewPlugin();
        }

        pluginTriggerActions.last()->setRequestPluginCallback([dockToViewPlugin, dockArea](PluginTriggerAction& pluginTriggerAction) -> void {
            plugins().requestViewPlugin(pluginTriggerAction.getPluginFactory()->getKind(), dockToViewPlugin, dockArea);
        });
    }

    sortActions(pluginTriggerActions);

    return pluginTriggerActions;
}
