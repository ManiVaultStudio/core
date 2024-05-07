// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningPageMiscellaneousWidget.h"
#include "LearningPageContentWidget.h"

#include <Application.h>

#include <QDebug>

LearningPageMiscellaneousWidget::LearningPageMiscellaneousWidget(LearningPageContentWidget* learningPageContentWidget) :
    QWidget(learningPageContentWidget),
    _learningPageContentWidget(learningPageContentWidget)
{
}
