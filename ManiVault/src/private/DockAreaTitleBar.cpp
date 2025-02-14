// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DockAreaTitleBar.h"
#include "DockManager.h"
#include "ViewMenu.h"
#include "LoadSystemViewMenu.h"

#include <CoreInterface.h>

#include <util/StyledIcon.h>

#include <QToolButton>

using namespace ads;
using namespace mv;
using namespace mv::util;
using namespace mv::gui;

DockAreaTitleBar::DockAreaTitleBar(ads::CDockAreaWidget* dockAreaWidget) :
    CDockAreaTitleBar(dockAreaWidget),
    _addViewPluginToolButton(new QToolButton(dockAreaWidget)),
    _viewMenu(nullptr),
    _loadSystemViewMenu(nullptr)
{
    _addViewPluginToolButton->setToolTip(QObject::tr("Add views"));
    _addViewPluginToolButton->setAutoRaise(true);
    _addViewPluginToolButton->setPopupMode(QToolButton::InstantPopup);
    _addViewPluginToolButton->setStyleSheet("QToolButton::menu-indicator { image: none; }");
    _addViewPluginToolButton->setIconSize(QSize(8, 8));

    auto dockManager = dockAreaWidget->dockManager();

    if (dockManager->objectName() == "MainDockManager") {
        const auto addMenu = [this, dockAreaWidget]() -> void {
            if (_loadSystemViewMenu != nullptr)
                return;

            _loadSystemViewMenu = new LoadSystemViewMenu(dockAreaWidget, dockAreaWidget);

            _addViewPluginToolButton->setMenu(_loadSystemViewMenu);
        };

        const auto updateToolButtonReadOnly = [this]() -> void {
            if (core()->isAboutToBeDestroyed())
                return;

            _addViewPluginToolButton->setEnabled(_loadSystemViewMenu->mayProducePlugins());
        };

        connect(dockAreaWidget->dockManager(), &CDockManager::dockWidgetAdded, this, updateToolButtonReadOnly);
        connect(dockAreaWidget->dockManager(), &CDockManager::dockWidgetAdded, this, addMenu);
        connect(dockAreaWidget->dockManager(), &CDockManager::stateRestored, this, addMenu);
        connect(dockAreaWidget->dockManager(), &CDockManager::dockWidgetRemoved, this, updateToolButtonReadOnly);

        updateToolButtonReadOnly();
    }
    
    if (dockManager->objectName() == "ViewPluginsDockManager") {
        const auto addMenu = [this, dockAreaWidget]() -> void {
            if (_viewMenu != nullptr)
                return;

            _viewMenu = new ViewMenu(dockAreaWidget, ViewMenu::LoadViewPlugins, dockAreaWidget);

            _addViewPluginToolButton->setMenu(_viewMenu);
        };

        const auto updateToolButtonReadOnly = [this]() -> void {
            _addViewPluginToolButton->setEnabled(_viewMenu->mayProducePlugins());
        };

        connect(dockAreaWidget->dockManager(), &CDockManager::dockWidgetAdded, this, updateToolButtonReadOnly);
        connect(dockAreaWidget->dockManager(), &CDockManager::dockWidgetAdded, this, addMenu);
        connect(dockAreaWidget->dockManager(), &CDockManager::stateRestored, this, addMenu);
        connect(dockAreaWidget->dockManager(), &CDockManager::dockWidgetRemoved, this, updateToolButtonReadOnly);

        updateToolButtonReadOnly();
    }

    insertWidget(indexOf(button(ads::TitleBarButtonTabsMenu)) - 1, _addViewPluginToolButton);

    const auto updateReadOnly = [this]() -> void {
        const auto projectIsReadOnly = mv::projects().hasProject() && mv::projects().getCurrentProject()->getReadOnlyAction().isChecked();

        _addViewPluginToolButton->setVisible(!workspaces().getLockingAction().isLocked() && !projectIsReadOnly);
    };

    connect(&workspaces().getLockingAction().getLockedAction(), &ToggleAction::toggled, this, updateReadOnly);

    updateReadOnly();

    _addViewPluginToolButton->setIcon(StyledIcon("plus"));
}
