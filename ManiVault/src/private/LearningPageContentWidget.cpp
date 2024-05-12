// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningPageContentWidget.h"

#include <QDebug>

using namespace mv::gui;

LearningPageContentWidget::LearningPageContentWidget(QWidget* parent /*= nullptr*/) :
    PageContentWidget(Qt::Vertical, parent),
    _videosWidget(this),
    _pluginResourcesWidget(this),
    _showVideosAction(this, "Show videos", true),
    _showTutorialsAction(this, "Show tutorials", true),
    _showPluginResourcesAction(this, "Show plugin resources"),
    _settingsAction(this, "Page settings"),
    _toStartPageAction(this, "To start page"),
    _toolbarAction(this, "Toolbar settings")
{
    getRowsLayout().addWidget(&_videosWidget);
    getRowsLayout().addWidget(&_pluginResourcesWidget);

    _showVideosAction.setSettingsPrefix("LearningPage/ShowVideos");
    _showTutorialsAction.setSettingsPrefix("LearningPage/ShowTutorials");
    _showPluginResourcesAction.setSettingsPrefix("LearningPage/ShowPluginResources");

    const auto toggleSections = [this]() -> void {
        _videosWidget.setVisible(_showVideosAction.isChecked());
        _pluginResourcesWidget.setVisible(_showPluginResourcesAction.isChecked());
    };

    toggleSections();

    connect(&_showVideosAction, &ToggleAction::toggled, this, toggleSections);
    //connect(&_showTutorialsAction, &ToggleAction::toggled, this, toggleViews);
    connect(&_showPluginResourcesAction, &ToggleAction::toggled, this, toggleSections);

    _settingsAction.setToolTip("Adjust page settings");
    _settingsAction.setIconByName("cog");

    _toStartPageAction.setIconByName("door-open");
    _toStartPageAction.setToolTip("Go to the start page");
    _toStartPageAction.setDefaultWidgetFlags(TriggerAction::Icon);

    _settingsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);

    _settingsAction.addAction(&_showVideosAction);
    //_settingsAction.addAction(&_showTutorialsAction);
    _settingsAction.addAction(&_showPluginResourcesAction);

    _toolbarAction.setShowLabels(false);

    _toolbarAction.addAction(new StretchAction(this, "Left stretch"));
    _toolbarAction.addAction(&_settingsAction);
    _toolbarAction.addAction(&_toStartPageAction);

    getMainLayout().addWidget(_toolbarAction.createWidget(this));

    connect(&_toStartPageAction, &TriggerAction::triggered, this, []() -> void {
        mv::help().getShowLearningCenterAction().setChecked(false);
        mv::projects().getShowStartPageAction().setChecked(true);\
    });
}
