// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StartPageContentWidget.h"
#include "StartPageWidget.h"

#include "PageAction.h"

#include <CoreInterface.h>

#include <QDebug>

using namespace mv;
using namespace mv::gui;

StartPageContentWidget::StartPageContentWidget(QWidget* parent /*= nullptr*/) :
    PageContentWidget(Qt::Horizontal, parent),
    _compactViewAction(this, "Compact"),
    _toggleOpenCreateProjectAction(this, "Open & Create", true),
    _toggleRecentProjectsAction(this, "Recent Projects", true),
    _toggleProjectFromWorkspaceAction(this, "Project From Workspace"),
    _toggleProjectFromDataAction(this, "Project From Data", true),
    _toggleTutorialsAction(this, "Tutorials", true),
    _settingsAction(this, "Settings"),
    _toLearningCenterAction(this, "Learning center"),
    _toolbarAction(this, "Toolbar settings"),
    _openProjectWidget(this),
    _getStartedWidget(this)
{
    _compactViewAction.setSettingsPrefix("StartPage/ToggleCompactView");
    _toggleOpenCreateProjectAction.setSettingsPrefix("StartPage/ToggleOpenCreateProject");
    _toggleRecentProjectsAction.setSettingsPrefix("StartPage/ToggleRecentProjects");
    _toggleProjectFromWorkspaceAction.setSettingsPrefix("StartPage/ToggleProjectFromWorkspace");
    _toggleProjectFromDataAction.setSettingsPrefix("StartPage/ToggleProjectFromData");
    _toggleTutorialsAction.setSettingsPrefix("StartPage/ToggleTutorials");

    _toggleProjectFromWorkspaceAction.setEnabled(false);

    _settingsAction.setText("Toggle Views");
    _settingsAction.setToolTip("Adjust page settings");
    _settingsAction.setIconByName("cog");

    _toLearningCenterAction.setIconByName("chalkboard-teacher");
    _toLearningCenterAction.setToolTip("Go to the learning center");
    _toLearningCenterAction.setDefaultWidgetFlags(TriggerAction::Icon);

    _settingsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);

    _settingsAction.addAction(&_toggleOpenCreateProjectAction);
    _settingsAction.addAction(&_toggleRecentProjectsAction);
    _settingsAction.addAction(&_toggleProjectFromWorkspaceAction);
    _settingsAction.addAction(&_toggleProjectFromDataAction);
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
}

void StartPageContentWidget::updateActions()
{
    PageAction::setCompactView(_compactViewAction.isChecked());

    _openProjectWidget.updateActions();
    _getStartedWidget.updateActions();
}
