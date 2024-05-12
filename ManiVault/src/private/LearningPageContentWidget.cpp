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
    _showVideosAction(this, "Show videos"),
    _showTutorialsAction(this, "Show tutorials"),
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

    _settingsAction.setToolTip("Adjust page settings");
    _settingsAction.setIconByName("cog");

    _toStartPageAction.setIconByName("door-open");
    _toStartPageAction.setToolTip("Go to the start page");

    _settingsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);

    _settingsAction.addAction(&_showVideosAction);
    //_settingsAction.addAction(&_showTutorialsAction);
    _settingsAction.addAction(&_showPluginResourcesAction);

    _toolbarAction.addAction(new StretchAction(this, "Left stretch"));
    _toolbarAction.addAction(&_settingsAction);
    _toolbarAction.addAction(&_toStartPageAction);

    getMainLayout().addWidget(_toolbarAction.createWidget(this));

    connect(&_toStartPageAction, &TriggerAction::triggered, this, []() -> void {
        mv::projects().getShowStartPageAction().setChecked(true);
    });
}
