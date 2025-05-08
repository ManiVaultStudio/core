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
    _toggleOpenCreateProjectSectionAction(this, "Open & Create", true),
    _toggleProjectDatabaseSectionAction(this, "Project Database", true),
    _toggleRecentProjectsSectionAction(this, "Recent Projects", true),
	_toggleProjectFromDataSectionAction(this, "Project From Data", true),
    _toggleProjectFromWorkspaceSectionAction(this, "Project From Workspace"),
    _toggleTutorialsSectionAction(this, "Tutorials", true),
    _settingsAction(this, "Settings"),
    _toLearningCenterAction(this, "Learning center"),
    _toolbarAction(this, "Toolbar settings"),
    _openProjectWidget(this),
    _getStartedWidget(this)
{
    if (!QFileInfo("StartPage.json").exists()) {
        _compactViewAction.setSettingsPrefix("StartPage/ToggleCompactView");
        _toggleOpenCreateProjectSectionAction.setSettingsPrefix("StartPage/ToggleOpenCreateProject");
        _toggleProjectDatabaseSectionAction.setSettingsPrefix("StartPage/ToggleProjectsRepository");
        _toggleRecentProjectsSectionAction.setSettingsPrefix("StartPage/ToggleRecentProjects");
        _toggleProjectFromWorkspaceSectionAction.setSettingsPrefix("StartPage/ToggleProjectFromWorkspace");
        _toggleProjectFromDataSectionAction.setSettingsPrefix("StartPage/ToggleProjectFromData");
        _toggleTutorialsSectionAction.setSettingsPrefix("StartPage/ToggleTutorials");

        _getStartedWidget.getTutorialsWidget().getTutorialsFilterModel().getTagsFilterAction().setSelectedOptions({ "GettingStarted" });
    } else {
        _settingsAction.setVisible(false);
        _toLearningCenterAction.setVisible(false);
    }

    _toggleProjectFromWorkspaceSectionAction.setEnabled(false);

    _settingsAction.setText("Toggle Views");
    _settingsAction.setToolTip("Adjust page settings");
    _settingsAction.setIconByName("toggle-on");

    _toLearningCenterAction.setIconByName("chalkboard-user");
    _toLearningCenterAction.setToolTip("Go to the learning center");
    _toLearningCenterAction.setDefaultWidgetFlags(TriggerAction::Icon);

    _settingsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);

    _settingsAction.addAction(&_toggleOpenCreateProjectSectionAction);

    if (QFileInfo("StartPage.json").exists()) {
        _settingsAction.addAction(&_toggleProjectDatabaseSectionAction);
    }

    _settingsAction.addAction(&_toggleRecentProjectsSectionAction);
    _settingsAction.addAction(&_toggleProjectFromDataSectionAction);

    // Disable until the project from workspace action is implemented properly
    //_settingsAction.addAction(&_toggleProjectFromWorkspaceSectionAction);

    _settingsAction.addAction(&_toggleTutorialsSectionAction);
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
        _toggleTutorialsSectionAction.setVisible(tutorialsAppFeatureEnabledAction.isChecked());
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
    _toggleOpenCreateProjectSectionAction.fromParentVariantMap(variantMap);
    _toggleProjectDatabaseSectionAction.fromParentVariantMap(variantMap);
    _toggleRecentProjectsSectionAction.fromParentVariantMap(variantMap);
    _toggleProjectFromDataSectionAction.fromParentVariantMap(variantMap);
    _toggleProjectFromWorkspaceSectionAction.fromParentVariantMap(variantMap);
    _toggleTutorialsSectionAction.fromParentVariantMap(variantMap);
    _openProjectWidget.fromParentVariantMap(variantMap);
    _getStartedWidget.fromParentVariantMap(variantMap);
}

QVariantMap StartPageContentWidget::toVariantMap() const
{
	auto variantMap = Serializable::toVariantMap();

    _compactViewAction.insertIntoVariantMap(variantMap);
    _toggleOpenCreateProjectSectionAction.insertIntoVariantMap(variantMap);
    _toggleProjectDatabaseSectionAction.insertIntoVariantMap(variantMap);
    _toggleRecentProjectsSectionAction.insertIntoVariantMap(variantMap);
    _toggleProjectFromDataSectionAction.insertIntoVariantMap(variantMap);
    _toggleProjectFromWorkspaceSectionAction.insertIntoVariantMap(variantMap);
    _toggleTutorialsSectionAction.insertIntoVariantMap(variantMap);
    _openProjectWidget.insertIntoVariantMap(variantMap);
    _getStartedWidget.insertIntoVariantMap(variantMap);

    return variantMap;
}
