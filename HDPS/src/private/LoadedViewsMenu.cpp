// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LoadedViewsMenu.h"

#include <Application.h>
#include <CoreInterface.h>
#include <ViewPlugin.h>

#include <actions/PluginTriggerAction.h>

#include <util/Miscellaneous.h>

using namespace hdps;
using namespace hdps::plugin;
using namespace hdps::util;
using namespace hdps::gui;

LoadedViewsMenu::LoadedViewsMenu(QWidget *parent /*= nullptr*/) :
    QMenu(parent)
{
    setTitle("Toggle");
    setToolTip("Toggle loaded view plugin visibility");
    setEnabled(!plugins().getPluginsByType(plugin::Type::VIEW).empty());
    setIcon(Application::getIconFont("FontAwesome").getIcon("low-vision"));

    _loadedSystemViewsMenu = QSharedPointer<QMenu>(new QMenu("System", this));

    populate();
}

void LoadedViewsMenu::populate()
{
    clear();

    for (auto& action : getLoadedViewsActions(false))
        addAction(action);

    if (!actions().isEmpty())
        addSeparator();

    const auto loadedViewActions = getLoadedViewsActions(true);

    if (!loadedViewActions.isEmpty()) {
        setEnabled(true);
        _loadedSystemViewsMenu->clear();

        for (auto& loadedSystemViewAction : loadedViewActions)
            _loadedSystemViewsMenu->addAction(loadedSystemViewAction);

        _loadedSystemViewsMenu->setIcon(Application::getIconFont("FontAwesome").getIcon("cogs"));

        addMenu(_loadedSystemViewsMenu.get());
    }
    else
        setEnabled(false);
}

QVector<QPointer<ToggleAction>> LoadedViewsMenu::getLoadedViewsActions(bool systemView)
{
    QVector<QPointer<ToggleAction>> actions;

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

        auto action = new ToggleAction(this, viewPlugin->text());

        action->setIcon(viewPlugin->getVisibleAction().icon());
        action->setChecked(viewPlugin->getVisibleAction().isChecked());

        connect(action, &QAction::toggled, this, [viewPlugin, action]() -> void {
            viewPlugin->getVisibleAction().toggle();
            });

        actions << action;
    }

    sortActions(actions);

    return actions;
}
