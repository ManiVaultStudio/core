// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StartPageConfigurationAction.h"

using namespace mv::util;

namespace mv::gui {

StartPageConfigurationAction::StartPageConfigurationAction(QObject* parent, const QString& title) :
    VerticalGroupAction(parent, title),
    _compactViewAction(this, "Compact"),
    _toggleOpenCreateProjectAction(this, "Open & Create", true),
    _toggleProjectDatabaseAction(this, "Project Database", true),
    _toggleRecentProjectsAction(this, "Recent Projects", true),
    _toggleProjectFromDataAction(this, "Project From Data", true),
    _toggleProjectFromWorkspaceAction(this, "Project From Workspace"),
    _toggleTutorialsAction(this, "Tutorials", true),
    _settingsGroupAction(this, "Settings", true),
    _toggleCustomizationAction(this, "Customization")
{
    setIconByName("gear");

	_settingsGroupAction.setToolTip("Adjust page settings");
    _settingsGroupAction.setIconByName("toggle-on");
    _settingsGroupAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);

	if (!Application::hasConfigurationFile()) {
        _compactViewAction.setSettingsPrefix("StartPage/ToggleCompactView");
        _toggleOpenCreateProjectAction.setSettingsPrefix("StartPage/ToggleOpenCreateProject");
        _toggleProjectDatabaseAction.setSettingsPrefix("StartPage/ToggleProjectsRepository");
        _toggleRecentProjectsAction.setSettingsPrefix("StartPage/ToggleRecentProjects");
        _toggleProjectFromWorkspaceAction.setSettingsPrefix("StartPage/ToggleProjectFromWorkspace");
        _toggleProjectFromDataAction.setSettingsPrefix("StartPage/ToggleProjectFromData");
        _toggleTutorialsAction.setSettingsPrefix("StartPage/ToggleTutorials");
    }
    else {
        //setVisible(false);
        //_toLearningCenterAction.setVisible(false);
    }

    _toggleProjectFromWorkspaceAction.setEnabled(false);

    addAction(&_compactViewAction);
    addAction(&_toggleOpenCreateProjectAction);
    addAction(&_toggleProjectDatabaseAction);
    addAction(&_toggleRecentProjectsAction);
    addAction(&_toggleProjectFromDataAction);
    addAction(&_toggleProjectFromWorkspaceAction);
    addAction(&_toggleTutorialsAction);
    // addAction(&_toggleProjectFromWorkspaceAction); Disable until the project from workspace action is implemented properly
    addAction(&_toggleCustomizationAction);

    _settingsGroupAction.addAction(&_compactViewAction);
    _settingsGroupAction.addAction(&_toggleOpenCreateProjectAction);
    _settingsGroupAction.addAction(&_toggleProjectDatabaseAction);
    _settingsGroupAction.addAction(&_toggleRecentProjectsAction);
    _settingsGroupAction.addAction(&_toggleProjectFromDataAction);
    _settingsGroupAction.addAction(&_toggleProjectFromWorkspaceAction);
    _settingsGroupAction.addAction(&_toggleTutorialsAction);

    const auto toggleCustomization = [this]() -> void {
        _settingsGroupAction.setVisible(_toggleCustomizationAction.isChecked());
	};

    toggleCustomization();

    connect(&_toggleCustomizationAction, &ToggleAction::toggled, this, toggleCustomization);
}

void StartPageConfigurationAction::fromVariantMap(const QVariantMap& variantMap)
{
	VerticalGroupAction::fromVariantMap(variantMap);

    _compactViewAction.fromParentVariantMap(variantMap, true);
    _toggleOpenCreateProjectAction.fromParentVariantMap(variantMap, true);
    _toggleProjectDatabaseAction.fromParentVariantMap(variantMap, true);
    _toggleRecentProjectsAction.fromParentVariantMap(variantMap, true);
    _toggleProjectFromWorkspaceAction.fromParentVariantMap(variantMap, true);
    _toggleProjectFromDataAction.fromParentVariantMap(variantMap, true);
    _toggleTutorialsAction.fromParentVariantMap(variantMap, true);
}

QVariantMap StartPageConfigurationAction::toVariantMap() const
{
    auto variantMap = VerticalGroupAction::toVariantMap();

    _compactViewAction.insertIntoVariantMap(variantMap);
    _toggleOpenCreateProjectAction.insertIntoVariantMap(variantMap);
    _toggleProjectDatabaseAction.insertIntoVariantMap(variantMap);
    _toggleRecentProjectsAction.insertIntoVariantMap(variantMap);
    _toggleProjectFromWorkspaceAction.insertIntoVariantMap(variantMap);
    _toggleProjectFromDataAction.insertIntoVariantMap(variantMap);
    _toggleTutorialsAction.insertIntoVariantMap(variantMap);

    return variantMap;
}
}
