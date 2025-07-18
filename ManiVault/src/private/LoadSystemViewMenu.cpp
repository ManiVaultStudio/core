// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LoadSystemViewMenu.h"
#include "PluginManager.h"
#include "ViewPluginDockWidget.h"

#include <Application.h>
#include <CoreInterface.h>
#include <AbstractWorkspaceManager.h>

#include <actions/PluginTriggerAction.h>

#include <util/Miscellaneous.h>

using namespace mv;
using namespace mv::plugin;
using namespace mv::gui;
using namespace mv::util;

LoadSystemViewMenu::LoadSystemViewMenu(QWidget *parent /*= nullptr*/, ads::CDockAreaWidget* dockAreaWidget /*= nullptr*/) :
    QMenu(parent),
    _dockAreaWidget(dockAreaWidget)
{
    setTitle("System views");
    setToolTip("Manage system view plugins");
    //setEnabled(mayProducePlugins());
    setIcon(StyledIcon("gears"));

    _loadViewsDockedMenus.insert(gui::DockAreaFlag::Left, QSharedPointer<QMenu>(new QMenu(gui::dockAreaMap.key(gui::DockAreaFlag::Left), this)));
    _loadViewsDockedMenus.insert(gui::DockAreaFlag::Right, QSharedPointer<QMenu>(new QMenu(gui::dockAreaMap.key(gui::DockAreaFlag::Right), this)));
    _loadViewsDockedMenus.insert(gui::DockAreaFlag::Top, QSharedPointer<QMenu>(new QMenu(gui::dockAreaMap.key(gui::DockAreaFlag::Top), this)));
    _loadViewsDockedMenus.insert(gui::DockAreaFlag::Bottom, QSharedPointer<QMenu>(new QMenu(gui::dockAreaMap.key(gui::DockAreaFlag::Bottom), this)));
    _loadViewsDockedMenus.insert(gui::DockAreaFlag::Center, QSharedPointer<QMenu>(new QMenu(gui::dockAreaMap.key(gui::DockAreaFlag::Center), this)));

    populate();
}

void LoadSystemViewMenu::populate()
{
    clear();

    if (_dockAreaWidget) {

        for (auto& dockArea : { gui::DockAreaFlag::Left, gui::DockAreaFlag::Right, gui::DockAreaFlag::Top, gui::DockAreaFlag::Bottom, gui::DockAreaFlag::Center })
        {
            auto& loadViewsDockedMenu = _loadViewsDockedMenus[dockArea];

            loadViewsDockedMenu->clear();
            loadViewsDockedMenu->setIcon(getDockAreaIcon(dockArea));

            for (auto& action : getLoadSystemViewsActions(dockArea))
                loadViewsDockedMenu->addAction(action);

            addMenu(loadViewsDockedMenu.get());
        }
    }
    else {
        const auto actions = getLoadSystemViewsActions();

        for (auto& action : actions)
            addAction(action);
    }
}

bool LoadSystemViewMenu::mayProducePlugins() const
{
    for (auto& pluginTriggerAction : plugins().getPluginTriggerActions(Type::VIEW)) {
        auto viewPluginFactory = dynamic_cast<const ViewPluginFactory*>(pluginTriggerAction->getPluginFactory());

        if (!viewPluginFactory->producesSystemViewPlugins())
            continue;

        if (pluginTriggerAction->isEnabled())
            return true;
    }

    return false;
}

QVector<QPointer<TriggerAction>> LoadSystemViewMenu::getLoadSystemViewsActions(mv::gui::DockAreaFlag dockArea /*= mv::gui::DockAreaFlag::None*/)
{
    QVector<QPointer<TriggerAction>> actions;

    auto pluginTriggerActions = plugins().getPluginTriggerActions(Type::VIEW);

    for (auto& pluginTriggerAction : pluginTriggerActions) {
        auto viewPluginFactory = dynamic_cast<const ViewPluginFactory*>(pluginTriggerAction->getPluginFactory());

        if (!viewPluginFactory->producesSystemViewPlugins())
            continue;

        if (!viewPluginFactory->getAllowPluginCreationFromStandardGui())
            continue;

        auto action = new TriggerAction(this, viewPluginFactory->getKind());

        action->setIcon(pluginTriggerAction->icon());
        action->setEnabled(pluginTriggerAction->isEnabled());

        connect(pluginTriggerAction, &PluginTriggerAction::enabledChanged, action, [action, pluginTriggerAction](bool enabled) -> void {
            if (enabled != action->isEnabled())
                action->setEnabled(enabled);
        });

        ViewPlugin* dockToViewPlugin = nullptr;

        if (_dockAreaWidget && _dockAreaWidget->dockWidgetsCount() >= 1) {
            auto dockWidgets = _dockAreaWidget->dockWidgets();

            if (auto firstViewPluginDockWidget = dynamic_cast<ViewPluginDockWidget*>(_dockAreaWidget->dockWidgets().first()))
                dockToViewPlugin = firstViewPluginDockWidget->getViewPlugin();
        }

        connect(action, &QAction::triggered, this, [pluginTriggerAction, dockToViewPlugin, viewPluginFactory, dockArea]() -> void {
            Application::core()->getPluginManager().requestViewPlugin(pluginTriggerAction->getPluginFactory()->getKind(), dockToViewPlugin, dockArea == DockAreaFlag::None ? viewPluginFactory->getPreferredDockArea() : dockArea);
        });

        actions << action;
    }

    sortActions(actions);

    return actions;
}
