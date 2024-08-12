// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningPageExamplesWidget.h"
#include "LearningPageContentWidget.h"

#include <QDebug>
#include <QStyledItemDelegate>

using namespace mv::gui;

LearningPageExamplesWidget::LearningPageExamplesWidget(LearningPageContentWidget* learningPageContentWidget) :
    QWidget(learningPageContentWidget),
    _learningPageContentWidget(learningPageContentWidget),
    _mainLayout()
{
    _mainLayout.setSpacing(20);
    _mainLayout.addWidget(PageContentWidget::createHeaderLabel("Examples", "Examples"));

    setLayout(&_mainLayout);

    updateCustomStyle();
}

bool LearningPageExamplesWidget::event(QEvent* event)
{
    if (event->type() == QEvent::ApplicationPaletteChange)
        updateCustomStyle();

    return QWidget::event(event);
}

void LearningPageExamplesWidget::updateCustomStyle()
{
}
