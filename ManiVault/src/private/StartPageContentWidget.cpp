// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StartPageContentWidget.h"
#include "StartPageWidget.h"

#include "PageAction.h"

#include <CoreInterface.h>

#include <QDebug>

using namespace mv;
using namespace mv::util;
using namespace mv::gui;

StartPageContentWidget::StartPageContentWidget(QWidget* parent /*= nullptr*/) :
    PageContentWidget(Qt::Horizontal, parent),
    Serializable("StartPageContentWidget"),
    _compactViewAction(this, "Compact"),
    _toggleOpenCreateProjectAction(this, "Open & Create", true),
    _toggleProjectDatabaseAction(this, "Project Database", true),
    _toggleRecentProjectsAction(this, "Recent Projects", true),
	_toggleProjectFromDataAction(this, "Project From Data", true),
    _toggleProjectFromWorkspaceAction(this, "Project From Workspace"),
    _toggleTutorialsAction(this, "Tutorials", true),
    _settingsAction(this, "Settings"),
    _toLearningCenterAction(this, "Learning center"),
    _toolbarAction(this, "Toolbar settings"),
    _openProjectWidget(this),
    _getStartedWidget(this)
{
    if (!QFileInfo("StartPage.json").exists()) {
        _compactViewAction.setSettingsPrefix("StartPage/ToggleCompactView");
        _toggleOpenCreateProjectAction.setSettingsPrefix("StartPage/ToggleOpenCreateProject");
        _toggleProjectDatabaseAction.setSettingsPrefix("StartPage/ToggleProjectsRepository");
        _toggleRecentProjectsAction.setSettingsPrefix("StartPage/ToggleRecentProjects");
        _toggleProjectFromWorkspaceAction.setSettingsPrefix("StartPage/ToggleProjectFromWorkspace");
        _toggleProjectFromDataAction.setSettingsPrefix("StartPage/ToggleProjectFromData");
        _toggleTutorialsAction.setSettingsPrefix("StartPage/ToggleTutorials");

        _getStartedWidget.getTutorialsWidget().getTutorialsFilterModel().getTagsFilterAction().setSelectedOptions({ "GettingStarted" });
    } else {
        _settingsAction.setVisible(false);
        _toLearningCenterAction.setVisible(false);
    }

    _toggleProjectFromWorkspaceAction.setEnabled(false);

    _settingsAction.setText("Toggle Views");
    _settingsAction.setToolTip("Adjust page settings");
    _settingsAction.setIconByName("toggle-on");

    _toLearningCenterAction.setIconByName("chalkboard-user");
    _toLearningCenterAction.setToolTip("Go to the learning center");
    _toLearningCenterAction.setDefaultWidgetFlags(TriggerAction::Icon);

    _settingsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);

    _settingsAction.addAction(&_toggleOpenCreateProjectAction);

    if (QFileInfo("StartPage.json").exists()) {
        _settingsAction.addAction(&_toggleProjectDatabaseAction);
    }

    _settingsAction.addAction(&_toggleRecentProjectsAction);
    _settingsAction.addAction(&_toggleProjectFromDataAction);

    // Disable until the project from workspace action is implemented properly
    //_settingsAction.addAction(&_toggleProjectFromWorkspaceAction);

    _settingsAction.addAction(&_toggleTutorialsAction);
    _settingsAction.addAction(&_compactViewAction);

    getColumnsLayout().addWidget(&_openProjectWidget);
    getColumnsLayout().addWidget(&_getStartedWidget);

    _toolbarAction.setShowLabels(false);
    _toolbarAction.setWidgetConfigurationFunction([](WidgetAction* action, QWidget* widget) -> void {
        widget->layout()->setContentsMargins(35, 10, 35, 20);
    });

    _toolbarAction.addAction(new StretchAction(this, "Left stretch"));
    _toolbarAction.addAction(&_settingsAction);
    _toolbarAction.addAction(&_toLearningCenterAction);
    _toolbarAction.addAction(&mv::projects().getBackToProjectAction());

    getMainLayout().addWidget(_toolbarAction.createWidget(this));

    connect(&_toLearningCenterAction, &TriggerAction::triggered, this, []() -> void {
        mv::projects().getShowStartPageAction().setChecked(false);
        mv::help().getShowLearningCenterPageAction().setChecked(true);
    });

    connect(&_compactViewAction, &ToggleAction::toggled, this, &StartPageContentWidget::updateActions);

    const auto& tutorialsAppFeatureEnabledAction = mv::constSettings().getAppFeaturesSettingsAction().getTutorialsAppFeatureAction().getEnabledAction();

    const auto updateTutorialsToggleVisibility = [this, &tutorialsAppFeatureEnabledAction]() -> void {
        _toggleTutorialsAction.setVisible(tutorialsAppFeatureEnabledAction.isChecked());
	};

    updateTutorialsToggleVisibility();

    connect(&tutorialsAppFeatureEnabledAction, &ToggleAction::toggled, this, updateTutorialsToggleVisibility);
}

void StartPageContentWidget::updateActions()
{
    PageAction::setCompactView(_compactViewAction.isChecked());

    _openProjectWidget.updateActions();
    _getStartedWidget.updateActions();
}

void StartPageContentWidget::fromVariantMap(const QVariantMap& variantMap)
{
	Serializable::fromVariantMap(variantMap);

    _compactViewAction.fromParentVariantMap(variantMap);
    _toggleOpenCreateProjectAction.fromParentVariantMap(variantMap);
    _toggleProjectDatabaseAction.fromParentVariantMap(variantMap);
    _toggleRecentProjectsAction.fromParentVariantMap(variantMap);
    _toggleProjectFromDataAction.fromParentVariantMap(variantMap);
    _toggleProjectFromWorkspaceAction.fromParentVariantMap(variantMap);
    _toggleTutorialsAction.fromParentVariantMap(variantMap);
    _openProjectWidget.fromParentVariantMap(variantMap);
    _getStartedWidget.fromParentVariantMap(variantMap);
}

QVariantMap StartPageContentWidget::toVariantMap() const
{
	auto variantMap = Serializable::toVariantMap();

    _compactViewAction.insertIntoVariantMap(variantMap);
    _toggleOpenCreateProjectAction.insertIntoVariantMap(variantMap);
    _toggleProjectDatabaseAction.insertIntoVariantMap(variantMap);
    _toggleRecentProjectsAction.insertIntoVariantMap(variantMap);
    _toggleProjectFromDataAction.insertIntoVariantMap(variantMap);
    _toggleProjectFromWorkspaceAction.insertIntoVariantMap(variantMap);
    _toggleTutorialsAction.insertIntoVariantMap(variantMap);
    _openProjectWidget.insertIntoVariantMap(variantMap);
    _getStartedWidget.insertIntoVariantMap(variantMap);

    return variantMap;
}
