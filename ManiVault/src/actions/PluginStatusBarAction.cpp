// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PluginStatusBarAction.h"

#include "CoreInterface.h"

#ifdef _DEBUG
    #define PLUGIN_STATUS_BAR_ACTION_VERBOSE
#endif

#include <QDebug>

using namespace mv::util;

namespace mv::gui {

PluginStatusBarAction::PluginStatusBarAction(QObject* parent, const QString& title, const QString& pluginKind /*= ""*/) :
    StatusBarAction(parent, title, mv::plugins().isPluginLoaded(pluginKind) ? StyledIcon(mv::plugins().getPluginFactory(pluginKind)->getIcon()) : StyledIcon()),
    _pluginKind(pluginKind),
    _conditionallyVisibleAction(this, "Only visible when instantiated", true)
{
    connect(&_conditionallyVisibleAction, &ToggleAction::toggled, this, &PluginStatusBarAction::updateConditionalVisibility);

    connect(&mv::plugins(), &AbstractPluginManager::pluginAdded, this, [this](plugin::Plugin* plugin) -> void {
        if (!_conditionallyVisibleAction.isChecked())
            return;

        if (plugin->getKind() != _pluginKind)
            return;

        updateConditionalVisibility();
    });

    connect(&mv::plugins(), &AbstractPluginManager::pluginAboutToBeDestroyed, this, [this](plugin::Plugin* plugin) -> void {
        if (!_conditionallyVisibleAction.isChecked())
            return;

        if (plugin->getKind() != _pluginKind)
            return;

        updateConditionalVisibility();
    });

    updateConditionalVisibility();
}

void PluginStatusBarAction::updateConditionalVisibility()
{
    if (_conditionallyVisibleAction.isChecked())
        setVisible(mv::plugins().getPluginFactory(_pluginKind)->getNumberOfInstances() >= 1);
    else
        setVisible(true);
}

}