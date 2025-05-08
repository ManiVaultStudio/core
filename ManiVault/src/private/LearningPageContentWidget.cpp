// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningPageContentWidget.h"

#include <CoreInterface.h>

#include <QDebug>

using namespace mv::gui;

LearningPageContentWidget::LearningPageContentWidget(QWidget* parent /*= nullptr*/) :
    PageContentWidget(Qt::Vertical, parent),
    _toggleVideosSectionAction(this, "Show videos", true),
    _toggleTutorialsSectionAction(this, "Show tutorials", true),
    _toggleExamplesSectionAction(this, "Show examples (coming soon)", false),
    _togglePluginResourcesSectionAction(this, "Show plugin resources", false),
    _settingsAction(this, "Page settings"),
    _toStartPageAction(this, "To start page"),
    _toolbarAction(this, "Toolbar settings"),
    _videosWidget(this),
    _tutorialsWidget(this),
    _examplesWidget(this),
    _pluginResourcesWidget(this)
{
    auto& tutorialsTagFilterAction = _tutorialsWidget.getTutorialsFilterModel().getTagsFilterAction();

    _tutorialsWidget.getHierarchyWidget().getToolbarAction().addAction(&tutorialsTagFilterAction);

    _tutorialsWidget.getTutorialsFilterModel().getTagsFilterAction().setSelectedOptions({ "GettingStarted" });

    auto& rowsLayout = getRowsLayout();

    rowsLayout.addWidget(&_videosWidget, 2);
    rowsLayout.addWidget(&_tutorialsWidget, 2);
    rowsLayout.addWidget(&_examplesWidget);
    rowsLayout.addWidget(&_pluginResourcesWidget);

    _toggleVideosSectionAction.setSettingsPrefix("LearningPage/ShowVideos");
    _toggleTutorialsSectionAction.setSettingsPrefix("LearningPage/ShowTutorials");
    _toggleExamplesSectionAction.setSettingsPrefix("LearningPage/ShowExamples");
    _togglePluginResourcesSectionAction.setSettingsPrefix("LearningPage/ShowPluginResources");

    _toggleExamplesSectionAction.setEnabled(false);

    const auto& appFeaturesSettingsAction           = mv::constSettings().getAppFeaturesSettingsAction();
    const auto& videosAppFeatureEnabledAction       = appFeaturesSettingsAction.getVideosAppFeatureAction().getEnabledAction();
    const auto& tutorialsAppFeatureEnabledAction    = appFeaturesSettingsAction.getTutorialsAppFeatureAction().getEnabledAction();

    const auto toggleSections = [this, &videosAppFeatureEnabledAction, &tutorialsAppFeatureEnabledAction]() -> void {
        _videosWidget.setVisible(videosAppFeatureEnabledAction.isChecked() && _toggleVideosSectionAction.isChecked());
        _tutorialsWidget.setVisible(tutorialsAppFeatureEnabledAction.isChecked() && _toggleTutorialsSectionAction.isChecked());
        _examplesWidget.setVisible(_toggleExamplesSectionAction.isChecked());
        _pluginResourcesWidget.setVisible(_togglePluginResourcesSectionAction.isChecked());
    };

    toggleSections();

    connect(&tutorialsAppFeatureEnabledAction, &ToggleAction::toggled, this, toggleSections);
    connect(&videosAppFeatureEnabledAction, &ToggleAction::toggled, this, toggleSections);

    connect(&_toggleVideosSectionAction, &ToggleAction::toggled, this, toggleSections);
    connect(&_toggleTutorialsSectionAction, &ToggleAction::toggled, this, toggleSections);
    connect(&_toggleExamplesSectionAction, &ToggleAction::toggled, this, toggleSections);
    connect(&_togglePluginResourcesSectionAction, &ToggleAction::toggled, this, toggleSections);

    _settingsAction.setToolTip("Adjust page settings");
    _settingsAction.setIconByName("gear");

    _toStartPageAction.setIconByName("door-open");
    _toStartPageAction.setToolTip("Go to the start page");
    _toStartPageAction.setDefaultWidgetFlags(TriggerAction::Icon);

    _settingsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _settingsAction.setPopupSizeHint(QSize(200, 200));

    _settingsAction.addAction(&_toggleVideosSectionAction);
    _settingsAction.addAction(&_toggleTutorialsSectionAction);
    _settingsAction.addAction(&_toggleExamplesSectionAction);
    _settingsAction.addAction(&_togglePluginResourcesSectionAction);

    _toolbarAction.setShowLabels(false);
    _toolbarAction.setWidgetConfigurationFunction([](WidgetAction* action, QWidget* widget) -> void {
        widget->layout()->setContentsMargins(35, 10, 35, 20);
    });

    _toolbarAction.addAction(&mv::help().getToDiscordAction());
    _toolbarAction.addAction(&mv::help().getToWebsiteAction());
    _toolbarAction.addAction(&mv::help().getToRepositoryAction());

    _toolbarAction.addAction(new StretchAction(this, "Stretch"));
    _toolbarAction.addAction(&_settingsAction);
    _toolbarAction.addAction(&_toStartPageAction);
    _toolbarAction.addAction(&mv::projects().getBackToProjectAction());

    getMainLayout().addWidget(_toolbarAction.createWidget(this));

    connect(&_toStartPageAction, &TriggerAction::triggered, this, []() -> void {
        mv::help().getShowLearningCenterPageAction().setChecked(false);
        mv::projects().getShowStartPageAction().setChecked(true);
    });

    const auto updateToggleSectionActionsVisibility = [this, &videosAppFeatureEnabledAction, &tutorialsAppFeatureEnabledAction]() -> void {
        _toggleVideosSectionAction.setVisible(videosAppFeatureEnabledAction.isChecked());
        _toggleTutorialsSectionAction.setVisible(tutorialsAppFeatureEnabledAction.isChecked());
    };

    updateToggleSectionActionsVisibility();

    connect(&videosAppFeatureEnabledAction, &ToggleAction::toggled, this, updateToggleSectionActionsVisibility);
    connect(&tutorialsAppFeatureEnabledAction, &ToggleAction::toggled, this, updateToggleSectionActionsVisibility);
}

