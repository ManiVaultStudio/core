// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningPageVideosWidget.h"
#include "LearningPageContentWidget.h"

#include <Application.h>

#include <QDebug>

LearningPageVideosWidget::LearningPageVideosWidget(LearningPageContentWidget* learningPageContentWidget) :
    QWidget(learningPageContentWidget),
    _learningPageContentWidget(learningPageContentWidget),
    _mainLayout(),
    _model(),
    _filterModel(),
    _hierarchyWidget(this, "Video", _model, &_filterModel)
{
    _mainLayout.addWidget(PageContentWidget::createHeaderLabel("Videos", "Videos"));
    _mainLayout.addWidget(&_hierarchyWidget);

    _hierarchyWidget.setWindowIcon(mv::Application::getIconFont("FontAwesome").getIcon("Video"));

    setLayout(&_mainLayout);
}
