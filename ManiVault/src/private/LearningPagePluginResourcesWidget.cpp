// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningPagePluginResourcesWidget.h"
#include "LearningPageContentWidget.h"
#include "PageContentWidget.h"
#include "LearningPagePluginAction.h"

#include <Application.h>

#include <QDebug>

using namespace mv::gui;

LearningPagePluginResourcesWidget::LearningPagePluginResourcesWidget(LearningPageContentWidget* learningPageContentWidget) :
    QWidget(learningPageContentWidget),
    _learningPageContentWidget(learningPageContentWidget),
    _mainLayout(),
    _pluginTypesFilterAction(this, "Plugin types", {}, {}),
    _pluginsLayout(0, 10, 10)
{
    setMinimumHeight(250);

    _mainLayout.setContentsMargins(0, 0, 0, 0);
    _mainLayout.setSpacing(20);
    _mainLayout.addWidget(PageContentWidget::createHeaderLabel("Plugin Resources", "Additional plugin information"));
    _mainLayout.addWidget(_pluginTypesFilterAction.createWidget(this, OptionsAction::Tags | OptionsAction::Selection));
    _mainLayout.addLayout(&_pluginsLayout, 1);

    _pluginsLayout.setContentsMargins(0, 0, 0, 0);

    QSet<QString> pluginTypesSet;

    for (auto pluginFactory : mv::plugins().getPluginFactoriesByTypes())
        pluginTypesSet.insert(getPluginTypeName(pluginFactory->getType()));

    QStringList pluginTypes(pluginTypesSet.begin(), pluginTypesSet.end());

    _pluginTypesFilterAction.initialize(pluginTypes, pluginTypes);
    _pluginTypesFilterAction.setSettingsPrefix("LearningPage/PluginResources/PluginTypesFilter");

    connect(&_pluginTypesFilterAction, &OptionsAction::selectedOptionsChanged, this, &LearningPagePluginResourcesWidget::updateFlowLayout);

    setLayout(&_mainLayout);

    updateFlowLayout();
}

void LearningPagePluginResourcesWidget::updateFlowLayout()
{
    QLayoutItem* layoutItem;

    while ((layoutItem = _pluginsLayout.takeAt(0)) != nullptr) {
        delete layoutItem->widget();
        delete layoutItem;
    }

    for (auto pluginFactory : mv::plugins().getPluginFactoriesByTypes())
        if (_pluginTypesFilterAction.getSelectedOptions().contains(getPluginTypeName(pluginFactory->getType())))
            _pluginsLayout.addWidget(new LearningPagePluginActionsWidget(pluginFactory));
}
