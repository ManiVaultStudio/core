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
    _toLearningCenterAction(this, "Learning center"),
    _toolbarAction(this, "Toolbar settings"),
    _openProjectWidget(this),
    _getStartedWidget(this)
{
    if (!Application::hasConfigurationFile()) {
        _getStartedWidget.getTutorialsWidget().getTutorialsFilterModel().getTagsFilterAction().setSelectedOptions({ "GettingStarted" });
    } else {
        _toLearningCenterAction.setVisible(false);
    }

    _toLearningCenterAction.setIconByName("chalkboard-user");
    _toLearningCenterAction.setToolTip("Go to the learning center");
    _toLearningCenterAction.setDefaultWidgetFlags(TriggerAction::Icon);

    getColumnsLayout().addWidget(&_openProjectWidget);
    getColumnsLayout().addWidget(&_getStartedWidget);

    _toolbarAction.setShowLabels(false);
    _toolbarAction.setWidgetConfigurationFunction([](WidgetAction* action, QWidget* widget) -> void {
        widget->layout()->setContentsMargins(35, 10, 35, 20);
    });

    auto& startPageConfigurationAction = Application::current()->getConfigurationAction().getStartPageConfigurationAction();

    _toolbarAction.addAction(new StretchAction(this, "Left stretch"));
    _toolbarAction.addAction(&startPageConfigurationAction.getSettingsGroupAction());
    _toolbarAction.addAction(&_toLearningCenterAction);
    _toolbarAction.addAction(&mv::projects().getBackToProjectAction());

    getMainLayout().addWidget(_toolbarAction.createWidget(this));

    connect(&_toLearningCenterAction, &TriggerAction::triggered, this, []() -> void {
        mv::projects().getShowStartPageAction().setChecked(false);
        mv::help().getShowLearningCenterPageAction().setChecked(true);
    });

    connect(&startPageConfigurationAction.getCompactViewAction(), &ToggleAction::toggled, this, &StartPageContentWidget::updateActions);
}

void StartPageContentWidget::updateActions()
{
    PageAction::setCompactView(Application::current()->getConfigurationAction().getStartPageConfigurationAction().getCompactViewAction().isChecked());

    _openProjectWidget.updateActions();
    _getStartedWidget.updateActions();
}
