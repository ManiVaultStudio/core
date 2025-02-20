// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LoadedViewsMenu.h"

#include <Application.h>
#include <CoreInterface.h>
#include <ViewPlugin.h>

#include <util/Miscellaneous.h>

using namespace mv;
using namespace mv::plugin;
using namespace mv::util;
using namespace mv::gui;

LoadedViewsMenu::LoadedViewsMenu(QWidget *parent /*= nullptr*/) :
    QMenu(parent)
{
    setTitle("Toggle");
    setToolTip("Toggle loaded view plugin visibility");
    setEnabled(!plugins().getPluginsByType(plugin::Type::VIEW).empty());
    setIcon(StyledIcon("low-vision"));

    _loadedSystemViewsMenu = QSharedPointer<QMenu>(new QMenu("System", this));

    populate();
}

void LoadedViewsMenu::populate()
{
    clear();

    for (auto& plugin : plugins().getPluginsByType(plugin::Type::VIEW)) {
        auto viewPlugin = dynamic_cast<ViewPlugin*>(plugin);

        disconnect(&viewPlugin->getVisibleAction(), &QAction::toggled, this, nullptr);
    }

    for (auto& viewToggleAction : _viewsToggleActions)
        delete viewToggleAction.get();

    auto loadedViewsAction = getLoadedViewsActions(false);

    _viewsToggleActions << loadedViewsAction;

    for (auto& viewToggleAction : getLoadedViewsActions(false))
        addAction(viewToggleAction.get());

    if (!actions().isEmpty())
        addSeparator();

    const auto loadedSystemViewActions = getLoadedViewsActions(true);

    _viewsToggleActions << loadedSystemViewActions;

    if (!loadedSystemViewActions.isEmpty()) {
        setEnabled(true);

        _loadedSystemViewsMenu->clear();

        for (auto& viewToggleAction : loadedSystemViewActions)
            _loadedSystemViewsMenu->addAction(viewToggleAction.get());

        _loadedSystemViewsMenu->setIcon(StyledIcon("cogs"));

        addMenu(_loadedSystemViewsMenu.get());
    }
    else
        setEnabled(false);
}

LoadedViewsMenu::ToggleActions LoadedViewsMenu::getLoadedViewsActions(bool systemView)
{
    ToggleActions actions;

    for (auto& plugin : plugins().getPluginsByType(plugin::Type::VIEW)) {
        auto viewPlugin = dynamic_cast<ViewPlugin*>(plugin);

        if (systemView) {
            if (!viewPlugin->isSystemViewPlugin())
                continue;
        }
        else {
            if (viewPlugin->isSystemViewPlugin())
                continue;
        }

        auto viewToggleAction = new ToggleAction(this, viewPlugin->text());

        //viewToggleAction->setIcon(viewPlugin->getVisibleAction().icon());

        const auto connectViewToggleAction = [this, viewPlugin, viewToggleAction]() -> void {
            connect(viewToggleAction, &QAction::toggled, this, [viewPlugin, viewToggleAction](bool toggled) -> void {
                viewPlugin->getVisibleAction().toggle();
            });
        };

        const auto disconnectViewToggleAction = [this, viewToggleAction]() -> void {
            disconnect(viewToggleAction, &QAction::toggled, this, nullptr);
        };

        const auto updateToggleActionChecked = [viewToggleAction, viewPlugin, connectViewToggleAction, disconnectViewToggleAction]() -> void {
            disconnectViewToggleAction();
            {
                viewToggleAction->setChecked(viewPlugin->getVisibleAction().isChecked());
            }
            connectViewToggleAction();
        };

        updateToggleActionChecked();

        connect(&viewPlugin->getVisibleAction(), &QAction::toggled, this, updateToggleActionChecked);

        connectViewToggleAction();

        actions << viewToggleAction;
    }

    sortActions(actions);

    return actions;
}
