// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningPageMiscellaneousWidget.h"
#include "LearningPageContentWidget.h"
#include "PageContentWidget.h"
#include "LearningPagePluginAction.h"

#include <Application.h>

#include <QDebug>

LearningPageMiscellaneousWidget::LearningPageMiscellaneousWidget(LearningPageContentWidget* learningPageContentWidget) :
    QWidget(learningPageContentWidget),
    _learningPageContentWidget(learningPageContentWidget),
    _mainLayout(),
    _pluginsLayout()
{
    _mainLayout.addWidget(PageContentWidget::createHeaderLabel("Plugin Resources", "Additional plugin information"));
    _mainLayout.addLayout(&_pluginsLayout);

    _pluginsLayout.setContentsMargins(0, 0, 0, 0);

    for (auto pluginFactory : mv::plugins().getPluginFactoriesByTypes())
        _pluginsLayout.addWidget(new LearningPagePluginActionsWidget(pluginFactory));

    setLayout(&_mainLayout);
}
