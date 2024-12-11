// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningPageTutorialDockWidget.h"

#ifdef _DEBUG
    //#define LEARNING_CENTER_TUTORIAL_DOCK_WIDGET_VERBOSE
#endif

using namespace ads;

using namespace mv;
using namespace mv::util;

LearningCenterTutorialDockWidget::LearningCenterTutorialDockWidget(const mv::util::LearningCenterTutorial& tutorial, QWidget* parent /*= nullptr*/) :
    CDockWidget(tutorial.getTitle(), parent),
    _learningCenterTutorial(tutorial)
{
    setMinimumSizeHintMode(CDockWidget::MinimumSizeHintFromContent);
    setWidget(&_tutorialView);

    _tutorialView.setUrl(tutorial.getUrl());
}

