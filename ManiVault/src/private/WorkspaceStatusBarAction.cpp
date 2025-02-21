// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkspaceStatusBarAction.h"

#include <actions/LockingAction.h>
#include <CoreInterface.h>

#ifdef _DEBUG
    #define WORKSPACE_STATUS_BAR_ACTION_VERBOSE
#endif

using namespace mv;
using namespace mv::gui;

WorkspaceStatusBarAction::WorkspaceStatusBarAction(QObject* parent, const QString& title) :
    StatusBarAction(parent, title, "table"),
    _settingsAction(this, "Workspace settings"),
    _layoutLockedAction(this, "Layout locked"),
    _moreSettingsAction(this, "More settings")
{
    setPopupAction(&_settingsAction);
    setToolTip("Explore workspace settings");

    _settingsAction.addAction(&_layoutLockedAction);
    _settingsAction.addAction(&_moreSettingsAction);

    _settingsAction.setShowLabels(false);
    //_settingsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::NoGroupBoxInPopupLayout);
    _settingsAction.setDefaultWidgetFlag(GroupAction::WidgetFlag::NoMargins);
    _settingsAction.setPopupSizeHint(QSize(250, 0));

    _layoutLockedAction.setToolTip("Toggle workspace layout locking");
    _layoutLockedAction.setStretch(1);

    _moreSettingsAction.setIconByName("ellipsis");
    _moreSettingsAction.setDefaultWidgetFlags(TriggerAction::WidgetFlag::Icon);
    _moreSettingsAction.setToolTip("More settings...");

    const auto updateLayoutLockedAction = [this]() -> void {
        _layoutLockedAction.setChecked(!(mv::workspaces().hasWorkspace() && workspaces().getLockingAction().getLockedAction().isChecked()));
        _moreSettingsAction.setVisible(!(mv::projects().hasProject() && mv::projects().getCurrentProject()->getReadOnlyAction().isChecked()));
    };

    updateLayoutLockedAction();

    connect(&mv::workspaces(), &AbstractWorkspaceManager::workspaceCreated, this, updateLayoutLockedAction);

    connect(&_layoutLockedAction, &ToggleAction::toggled, this, [this](bool toggled) -> void {
        workspaces().getLockingAction().getLockedAction().setChecked(_layoutLockedAction.isChecked());
    });

    connect(&_moreSettingsAction, &TriggerAction::triggered, this, [this]() -> void {
        projects().getEditProjectSettingsAction().trigger();
    });
}
